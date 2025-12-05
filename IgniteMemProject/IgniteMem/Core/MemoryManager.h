#pragma once

#include <tuple>
#include <cassert>
#include <cstddef>

#include "Stack.h"
#include "Defines.h"
#include "ListNode.h"

#ifdef DEV_CONFIGURATION
#include <mutex>
#include <thread>
#include <unordered_map>
#include "DebugMemoryHexValues.h"

template <typename T>
struct CustomAllocator 
{
    typedef T value_type;

    CustomAllocator() = default;

    template <typename U>
    constexpr CustomAllocator(const CustomAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) 
    {
        void* address = malloc(n * sizeof(T));
        return static_cast<T*>(address);
    }

    void deallocate(T* p, std::size_t n) noexcept 
    {
        if (p != nullptr)
        {
            for (std::size_t i = 0; i < n; ++i)
            {
                p[i].~T();
            }

            free(p);
        }
    }
};

struct AllocationInfo
{
    int id;
    size_t size;
    char allocationLocation[256];
};

using AllocationMap = std::unordered_map<std::byte*, AllocationInfo, std::hash<std::byte*>, std::equal_to<std::byte*>, CustomAllocator<std::pair<std::byte* const, AllocationInfo>>>;
#endif // DEV_CONFIGURATION.

namespace ignite::mem
{

struct API MemoryBlock
{
    std::byte* address;
    uint64_t   sizeFree;
};

template class  API Stack<unsigned char>;
template struct API ListNode<MemoryBlock>;

class API MemoryManager
{
    using Stack    = Stack<unsigned char>;
    using ListNode = ListNode<MemoryBlock>;
public:
    [[nodiscard]] inline static MemoryManager* Instance() { return mInstance; }

    MemoryManager(const MemoryManager&)            = delete;
    MemoryManager(MemoryManager&&)                 = delete;
    MemoryManager& operator=(MemoryManager&&)      = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    static void Init(const uint32_t sizeBytes) noexcept;
    static void Destroy() noexcept;

#ifdef DEV_CONFIGURATION
    [[nodiscard]] void* New(const uint32_t size, const char* name) noexcept;
#else // Else DEV_CONFIGURATION
    [[nodiscard]] void* New(const uint32_t size);
#endif // !DEV_CONFIGURATION

    template <typename T, typename... Args>
    [[deprecated("This should not be used in runtime applications, it is kept for the purpose of tests.")]]
    [[nodiscard]] T* New(Args ...args) noexcept;

    template <typename T>
    void Delete(T* free) noexcept;

    //Todo Ideally, I don't want to expose these, but these are used in tests.
    [[nodiscard]] inline static uint32_t GetMetadataPadding() { return METADATA_PADDING; }
    [[nodiscard]] inline uint64_t        GetSizeFree()  const { return mSize - mAllocated; }

#ifdef DEV_CONFIGURATION
    [[nodiscard]] inline ListNode*     GetStartingListNode()   const { return mStartingListNode; }
    [[nodiscard]] inline const Stack&  GetListStack()          const { return mListNodeFreeIndicesStack;  }
    [[nodiscard]] inline void*         GetStartOfMemoryBlock() const { return mMemoryBlock; }
    [[nodiscard]] inline uint64_t      GetSize()               const { return mSize; }
    [[nodiscard]] inline uint64_t      GetAllocated()          const { return mAllocated; }
    [[nodiscard]] inline uint32_t      GetNumberOfFragments()  const { return mNumberOfFragments; }
    [[nodiscard]] inline AllocationMap GetAllocationMap()            { AllocationMap copy; { std::scoped_lock lock(mThreadMutex); copy = mAddressToAllocInfo; } return copy; }

    static void SetMemoryBlockDebug(DebugMemoryHexValues value, void* memory, const uint64_t size);

    Log logger{ "IgniteMem" };
private:
    std::mutex  mThreadMutex;
    std::thread mThread;
    uint32_t    mNumberOfFragments{ 1 };

    AllocationMap mAddressToAllocInfo;
public:
    friend class DebugMemoryConsole;
#endif // DEV_CONFIGURATION.

private:
#ifdef DEV_CONFIGURATION
    static constexpr uint32_t MAX_FRAGMENTS   { 128 };
#else // Else DEV_CONFIGURATION.
    static constexpr uint32_t MAX_FRAGMENTS   {  64 };
#endif // !DEV_CONFIGURATION.
    static constexpr uint32_t METADATA_PADDING{   4 };

    MemoryManager(const uint64_t sizeBytes) noexcept;
    ~MemoryManager() noexcept;

    static MemoryManager* mInstance;

    void*    mMemoryBlock;
    uint64_t mSize;
    uint64_t mAllocated{};

    ListNode* mStartingListNode;

    Stack     mListNodeFreeIndicesStack{ MAX_FRAGMENTS };
    ListNode* mListNodes;

    std::tuple<ListNode*, ListNode*> FindAllocationListNode(const uint64_t size) const;
    ListNode* GetListNode(std::byte* address, const uint64_t size, ListNode* next);
    void FreeListNode(const ListNode* node);
};

/*
   =================================================================================================
                                                                                                      */

template <typename T, typename... Args>
T* MemoryManager::New(Args ...args) noexcept
{
#ifdef DEV_CONFIGURATION
    void* address = New(sizeof(T), "PlacementAllocation");
#else // Else DEV_CONFIGURATION.
    void* address = New(sizeof(T));
#endif // !DEV_CONFIGURATION.
    return new (static_cast<T*>(address)) T{ std::forward<Args>(args)... };
}

template <typename T>
void MemoryManager::Delete(T* free) noexcept
{
    DEBUG(std::scoped_lock lock(mThreadMutex));
    assert(free);

#ifdef DEV_CONFIGURATION
    mAddressToAllocInfo.erase((std::byte*)free);

    if ((std::byte*)free < mMemoryBlock || (std::byte*)free > (std::byte*)mMemoryBlock + mSize)
    {
        MEM_LOG_WARN("Freed memory that was allocated on heap due to exceeding memory budget.");
        delete free;
        return;
    }

    ListNode* freedAlreadyNode = mStartingListNode;
    while (freedAlreadyNode)
    {
        if ((std::byte*)free > freedAlreadyNode->value.address && (std::byte*)free < freedAlreadyNode->value.address + freedAlreadyNode->value.sizeFree)
        {
            MEM_LOG_ERROR("Trying to free a memory address that has already been freed!");
            return;
        }
        freedAlreadyNode = freedAlreadyNode->next;
    }
#endif // DEV_CONFIGURATION.

    free->~T();

    std::byte*     freeAddress      = (std::byte*)free - METADATA_PADDING;
    const uint64_t deallocationSize = *((uint32_t*)freeAddress) + METADATA_PADDING;

    DEBUG(SetMemoryBlockDebug(DebugMemoryHexValues::FREED, freeAddress, deallocationSize));

    mAllocated -= deallocationSize;

    // Keep track of the previous so we can correctly relink the list.
    ListNode* previous = nullptr;
    ListNode* node     = mStartingListNode;

    // Try to find the very first node that is strictly to the left of the free address.
    while (node->next && node->next->value.address < freeAddress)
    {
        previous = node;
        node     = node->next;
    }

    // When the memory block has been free and max has been allocated, we can re-use the full block to make a new block.
    if (node->value.sizeFree == 0)
    {
        // Since this should only occur when memory of reserved block is at full allocation, assert to catch any edge cases.
        assert(mAllocated + deallocationSize == mSize && "This should only exist if its the first node with no memory left");
        assert(!node->next);

        node->value.address  = freeAddress;
        node->value.sizeFree = deallocationSize;
        return;
    }

    // If there is no node to the left of the free address, we might need to create a new node.
    if (node->value.address > freeAddress)
    {
        // Check if the freed address is adjacent to the free block of memory on the right.
        if (freeAddress + deallocationSize == node->value.address)
        {
            node->value.address   = freeAddress;
            node->value.sizeFree += deallocationSize;
            return;
        }

        ListNode* newNode = GetListNode(freeAddress, deallocationSize, node);

        if (previous)
        {
            previous->next = newNode;
        }

        mStartingListNode = newNode;

        return;
    }

    // If our left node is adjacent to the freed block, we can immediately collapse into a single node, else create a new link.
    if (node->value.address + node->value.sizeFree == freeAddress)
    {
        node->value.sizeFree += deallocationSize;
    }
    else
    {
        ListNode* newNode = GetListNode(freeAddress, deallocationSize, node->next);

        node->next = newNode;

        node = newNode;
    }

    // Check if we can merge with the node to the right.
    if (node->next && node->value.address + node->value.sizeFree == node->next->value.address)
    {
        node->value.sizeFree += node->next->value.sizeFree;

        const ListNode* next = node->next;
        node->next = next->next;

        FreeListNode(next);
    }
}

} // Namespace ignite::mem.
