#include "MemoryManager.h"

#include <print>
#include <cassert>

#ifdef DEV_CONFIGURATION
#include "DebugMemoryConsole.h"
#endif // DEV_CONFIGURATION.

void* operator new(std::size_t size)
{
#ifndef DEV_CONFIGURATION
    std::println("Operator new used inside memory manager to allocate size {}. Note: This could be logging.", size);
#endif // !DEV_CONFIGURATION.
    return malloc(size);
}

void operator delete(void* address) noexcept
{
#ifndef DEV_CONFIGURATION
    std::println("Operator delete used inside memory manager to deallocate memory. Note: This could be logging.");
#endif // !DEV_CONFIGURATION.
    free(address);
}

namespace ignite::mem
{

MemoryManager* MemoryManager::mInstance = nullptr;

MemoryManager::MemoryManager(const uint64_t sizeBytes) noexcept
{
    mSize = sizeBytes;
    mMemoryBlock = malloc(sizeBytes);
    DEBUG(SetMemoryBlockDebug(DebugMemoryHexValues::UNALLOCATED, mMemoryBlock, mSize));

    // Populate the stack in reverse order to get index of 0 on first pop.
    for (unsigned char i{ 0 }; i < MAX_FRAGMENTS; ++i)
    {
        mListNodeFreeIndicesStack.push(MAX_FRAGMENTS - i - 1);
    }

    mListNodes = (ListNode*)malloc(sizeof(ListNode) * MAX_FRAGMENTS);

    mStartingListNode = new (&mListNodes[mListNodeFreeIndicesStack.pop()]) ListNode{ .value = {.address = static_cast<std::byte*>(mMemoryBlock), .sizeFree = sizeBytes }, .next = nullptr };

#ifdef DEV_LIVE_STATS
    mThread = std::thread(&DebugMemoryConsole::Init);
#endif // DEV_LIVE_STATS.
}

MemoryManager::~MemoryManager() noexcept
{
#ifdef DEV_CONFIGURATION
    if (mNumberOfFragments != 1)
    {
        MEM_LOG_WARN("Memory Manager being Destroyed and not all memory was freed, could be a potential memory leak if this was not intentional.");
    }
#endif // DEV_CONFIGURATION.

    free(mListNodes);
    free(mMemoryBlock);
}

void MemoryManager::Init(const uint32_t sizeBytes) noexcept
{
#ifdef DEV_CONFIGURATION
    if (mInstance)
    {
        MEM_LOG_ERROR("Trying to re-create memory manager when it already exists.");
        return;
    }
#endif // DEV_CONFIGURATION.

    void* memoryAddress = malloc(sizeof(MemoryManager));
    mInstance = new (memoryAddress) MemoryManager(sizeBytes);
    MEM_LOG_DEBUG("Memory managed successfully initialized.");
}

void MemoryManager::Destroy() noexcept
{
#ifdef DEV_LIVE_STATS
    // Since the debug console is constructed on a different thread, it's possible to try close before the instance has been constructed.
    // Force stall until it's been created to ensure resources freed.
    while (!DebugMemoryConsole::Instance()) { }
    DebugMemoryConsole::Instance()->StopRunning();

    if (mInstance->mThread.joinable())
    {
        mInstance->mThread.join();
    }
#endif // DEV_LIVE_STATS.

    free(mInstance);
    mInstance = nullptr;
    MEM_LOG_DEBUG("Memory managed successfully destroyed.");
}

void* MemoryManager::New(const uint64_t size) noexcept
{
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
        return malloc(size);
    }
#endif // DEV_CONFIGURATION.

#ifndef DEV_CONFIGURATION
    if (mSize - mAllocated < allocatedSize)
    {
        throw std::bad_alloc{ };
    }
#endif // DEV_CONFIGURATION.

    auto [previous, allocationNode] = FindAllocationListNode(allocatedSize);

    *reinterpret_cast<uint32_t*>(allocationNode->value.address) = size;

    mAllocated += allocatedSize;

    allocationNode->value.address += allocatedSize;
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

    return typeAddress;
}

#ifdef DEV_CONFIGURATION
void MemoryManager::SetMemoryBlockDebug(DebugMemoryHexValues value, void* memory, const uint64_t size)
{
    memset(memory, static_cast<int>(value), size);
}

#endif // DEV_CONFIGURATION.

std::tuple<MemoryManager::ListNode*, MemoryManager::ListNode*> MemoryManager::FindAllocationListNode(const uint64_t size) const
{
    ListNode* previous = nullptr;
    ListNode* node     = mStartingListNode;

    while (node->value.sizeFree < size)
    {
        previous = node;
        node = node->next;

#ifdef DEV_CONFIGURATION
        if (!node)
        {
            MEM_LOG_ERROR("Could not find a valid list node to allocate to.");
        }
#endif // DEV_CONFIGURATION.
    }

    return { previous, node };
}

MemoryManager::ListNode* MemoryManager::GetListNode(std::byte* address, const uint64_t size, ListNode* next)
{
#ifdef DEV_CONFIGURATION
    ++mNumberOfFragments;
    if (mListNodeFreeIndicesStack.empty())
    {
        MEM_LOG_ERROR("Memory is too fragmented and cannot create a new node. New node will be created on heap for debug mode, but needs to be addressed for release!");
        // We have failed to create a list node due to there being too many nodes. Create a new node on the heap as we are in debug mode.
        return new ListNode{ .value = {.address = address, .sizeFree = size }, .next = next };
    }

    const float percentage = 1.0f - static_cast<float>(mListNodeFreeIndicesStack.size()) / static_cast<float>(mListNodeFreeIndicesStack.capacity());
    if (percentage > 0.8f)
    {
        MEM_LOG_ERROR("Memory Fragmentation Budge: Memory budget is getting low, at {}% of fragments used!", percentage * 100.0f);
    }

    return new (&mListNodes[mListNodeFreeIndicesStack.pop()]) ListNode{ .value = {.address = address, .sizeFree = size }, .next = next };
#endif // DEV_CONFIGURATION.
    return new (&mListNodes[mListNodeFreeIndicesStack.pop()]) ListNode{ .value = {.address = address, .sizeFree = size }, .next = next };
}

void MemoryManager::FreeListNode(const ListNode* node)
{
    if (!node) { return; }

    const uint64_t addressDifference = node - mListNodes;
#ifdef DEV_CONFIGURATION
    --mNumberOfFragments;
    if (addressDifference > mListNodeFreeIndicesStack.capacity())
    {
        MEM_LOG_WARN("Freed list node that was created on the heap!");

        delete node;
        return;
    }
#endif // DEV_CONFIGURATION.

    mListNodeFreeIndicesStack.push(static_cast<unsigned char>(addressDifference));
}

} // Namespace ignite::mem.
