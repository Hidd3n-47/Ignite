#pragma once

#include <cassert>

#include "Defines.h"
#include "ListNode.h"

#ifdef DEV_CONFIGURATION
#include <mutex>
#include <thread>
#include "DebugMemoryHexValues.h"
#endif // DEV_CONFIGURATION.


#ifdef MEM_MANAGER
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif

namespace ignite::mem
{

struct MemoryBlock
{
    std::byte* address;
    uint64_t   sizeFree;
};

class API MemoryManager
{
    using ListNode = ListNode<MemoryBlock>;
public:
    [[nodiscard]] inline static MemoryManager* Instance() { return mInstance; }

    MemoryManager(const MemoryManager&)            = delete;
    MemoryManager(MemoryManager&&)                 = delete;
    MemoryManager& operator=(MemoryManager&&)      = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    static void Init(const uint32_t sizeBytes);
    static void Destroy();

    template <typename T>
    T* New();

    template <typename T>
    void Delete(T* free);

#ifdef DEV_CONFIGURATION
    [[nodiscard]] inline void*    GetStartOfMemoryBlock() const { return mMemoryBlock; }
    [[nodiscard]] inline uint64_t GetSize()               const { return mSize; }
    [[nodiscard]] inline uint64_t GetAllocated()          const { return mAllocated; }
    [[nodiscard]] inline uint64_t GetSizeFree()           const { return mSize - mAllocated; }

    static void SetMemoryBlockDebug(DebugMemoryHexValues value, void* memory, const uint64_t size);
    inline static uint32_t GetMetadataPadding() { return METADATA_PADDING; }

private:
    std::thread mThread;

    uint64_t mSize;
    uint64_t mAllocated{};
public:
    friend class DebugMemoryConsole;
#endif // DEV_CONFIGURATION.

private:
    MemoryManager(const uint64_t sizeBytes);
    ~MemoryManager();

    static MemoryManager* mInstance;

    void* mMemoryBlock;

    ListNode* mStartingListNode;

    static constexpr uint32_t METADATA_PADDING{ 4 };

    ListNode* FindAllocationListNode(const uint64_t size) const;
};

template <typename T>
T* MemoryManager::New()
{
    const uint64_t size = sizeof(T);
    const uint64_t allocatedSize = size + METADATA_PADDING;

    ListNode* allocationNode = FindAllocationListNode(allocatedSize);

    *reinterpret_cast<uint32_t*>(allocationNode->value.address) = size;

    mAllocated += allocatedSize;

    allocationNode->value.address  += allocatedSize;
    allocationNode->value.sizeFree -= allocatedSize;

    void* typeAddress = allocationNode->value.address - size;

    DEBUG(SetMemoryBlockDebug(DebugMemoryHexValues::NEWLY_ALLOCATED, typeAddress, size));

    return static_cast<T*>(typeAddress);
}

template <typename T>
void MemoryManager::Delete(T* free)
{
    std::byte*     freeAddress      = (std::byte*)free - METADATA_PADDING;
    const uint64_t deallocationSize = *((uint32_t*)freeAddress) + METADATA_PADDING;

    DEBUG(SetMemoryBlockDebug(DebugMemoryHexValues::FREED, freeAddress, deallocationSize));

    mAllocated -= deallocationSize;

    assert(freeAddress >= mMemoryBlock && "Trying to free memory address that is before the allocated block");
    assert(freeAddress <= (std::byte*)mMemoryBlock + mSize && "Trying to free memory address that is after the allocated block");

    // Keep track of the previous so we can correctly relink the list.
    ListNode* previous = nullptr;
    ListNode* node     = mStartingListNode;

    // Try to find the very first node that is strictly to the left of the free address.
    while (node->next && node->next->value.address < freeAddress)
    {
        previous = node;
        node     = node->next;
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

        ListNode* newNode = new ListNode{ .value = { .address = freeAddress, .sizeFree = deallocationSize }, .next = node };

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
        ListNode* newNode = new ListNode{ .value = {.address = freeAddress, .sizeFree = deallocationSize }, .next = node->next };

        node->next = newNode;

        node = newNode;
    }

    // Check if we can merge with the node to the right.
    if (node->next && node->value.address + node->value.sizeFree == node->next->value.address)
    {
        node->value.sizeFree += node->next->value.sizeFree;

        const ListNode* next = node->next;
        node->next = next->next;

        delete next;
    }
}

} // Namespace ignite::mem;