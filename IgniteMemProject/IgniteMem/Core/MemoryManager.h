#pragma once

#include <tuple>
#include <cassert>
#include <cstddef>
#include <exception>

#include "Stack.h"
#include "Defines.h"
#include "ListNode.h"

#ifdef DEV_CONFIGURATION
#include <mutex>
#include <thread>
#include "DebugMemoryHexValues.h"
#endif // DEV_CONFIGURATION.

namespace ignite::mem
{

struct MemoryBlock
{
    std::byte* address;
    uint64_t   sizeFree;
};

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

    static void Init(const uint32_t sizeBytes);
    static void Destroy();

    template <typename T, typename... Args>
    T* New(Args ...args) noexcept;

    template <typename T>
    void Delete(T* free) noexcept;

#ifdef DEV_CONFIGURATION
    [[nodiscard]] inline ListNode*    GetStartingListNode()   const { return mStartingListNode; }
    [[nodiscard]] inline const Stack& GetListStack()          const { return mListNodeFreeIndicesStack;  }
    [[nodiscard]] inline void*        GetStartOfMemoryBlock() const { return mMemoryBlock; }
    [[nodiscard]] inline uint64_t     GetSize()               const { return mSize; }
    [[nodiscard]] inline uint64_t     GetAllocated()          const { return mAllocated; }
    [[nodiscard]] inline uint64_t     GetSizeFree()           const { return mSize - mAllocated; }
    [[nodiscard]] inline uint32_t     GetNumberOfFragments()  const { return mNumberOfFragments; }

    static void SetMemoryBlockDebug(DebugMemoryHexValues value, void* memory, const uint64_t size);
    inline static uint32_t GetMetadataPadding() { return METADATA_PADDING; }

private:
    std::thread mThread;
    uint32_t    mNumberOfFragments{ 1 };
public:
    friend class DebugMemoryConsole;
#endif // DEV_CONFIGURATION.

private:
    static constexpr uint32_t MAX_FRAGMENTS   { 64 };
    static constexpr uint32_t METADATA_PADDING{  4 };

    MemoryManager(const uint64_t sizeBytes);
    ~MemoryManager();

    static MemoryManager* mInstance;

    void*    mMemoryBlock;
    uint64_t mSize;
    uint64_t mAllocated{};

    ListNode* mStartingListNode;

    Stack    mListNodeFreeIndicesStack{ MAX_FRAGMENTS };
    ListNode mListNodes[MAX_FRAGMENTS];

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
    const uint64_t size = sizeof(T);
    const uint64_t allocatedSize = size + METADATA_PADDING;

#ifdef DEV_CONFIGURATION
    const float percentage = static_cast<float>(mAllocated + allocatedSize) / mSize * 100.0f;
    if (percentage >= 80)
    {
        MEM_LOG_WARN("Memory Budged Warning: At {}% of memory budget.", percentage);
    }

    if (mSize - mAllocated < allocatedSize)
    {
        MEM_LOG_ERROR("Memory Budged Exceeded: Not enough reserved memory to allocate! Object will be heap allocated in debug but will fail to create in release builds.");
        return new T{ std::forward<Args>(args)... };
    }
#endif // DEV_CONFIGURATION.

#ifndef DEV_CONFIGURATION
    if (mSize - mAllocated < allocatedSize)
    {
        throw std::bad_alloc{ };
    }
#endif

    auto [previous, allocationNode] = FindAllocationListNode(allocatedSize);

    *reinterpret_cast<uint32_t*>(allocationNode->value.address) = size;

    mAllocated += allocatedSize;

    allocationNode->value.address  += allocatedSize;
    allocationNode->value.sizeFree -= allocatedSize;

    void* typeAddress = allocationNode->value.address - size;

    // If this allocation fills the block, it might be possible to remove the block.
    if (allocationNode->value.sizeFree == 0)
    {
        // If we are the starting block but have another block, we can remove the node and update starting node.
        if (!previous && mStartingListNode->next)
        {
            // If there is no previous node, we have to be the starting node, else we have a broken list.
            assert(allocationNode == mStartingListNode);
            mStartingListNode = allocationNode->next;

            FreeListNode(allocationNode);
        }

        // If we are not the starting block, we can delete the intermediate block and link the previous to next.
        if (previous)
        {
            previous->next = allocationNode->next;

            FreeListNode(allocationNode);
        }

        // The omitted case is when we are the starting node and have no next node.
        // We do not need to do anything in this situation as we need to constantly have a node.
        // Therefore, we just will have a node of 0 bytes and do not want to delete it.
    }

    T* allocation = new (static_cast<T*>(typeAddress)) T{ std::forward<Args>(args)... };

    return allocation;
}

template <typename T>
void MemoryManager::Delete(T* free) noexcept
{
    assert(free);

#ifdef DEV_CONFIGURATION
    if ((std::byte*)free < mMemoryBlock || (std::byte*)free > (std::byte*)mMemoryBlock + mSize)
    {
        //MEM_LOG_WARN("Freed memory that was allocated on heap due to exceeding memory budget.");
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

} // Namespace ignite::mem;