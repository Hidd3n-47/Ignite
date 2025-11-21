#include "MemoryManager.h"

#include <cassert>

#ifdef DEV_CONFIGURATION
#include "DebugMemoryConsole.h"
#endif // DEV_CONFIGURATION.

namespace ignite::mem
{

MemoryManager* MemoryManager::mInstance = nullptr;

MemoryManager::MemoryManager(const uint64_t sizeBytes)
{
    mSize = sizeBytes;
    mMemoryBlock = malloc(sizeBytes);
    DEBUG(SetMemoryBlockDebug(DebugMemoryHexValues::UNALLOCATED, mMemoryBlock, mSize));

    // Populate the stack in reverse order to get index of 0 on first pop.
    for (unsigned char i{ 0 }; i < MAX_FRAGMENTS; ++i)
    {
        mListNodeFreeIndicesStack.push(MAX_FRAGMENTS - i - 1);
    }

    mStartingListNode = new (&mListNodes[mListNodeFreeIndicesStack.pop()]) ListNode{ .value = {.address = static_cast<std::byte*>(mMemoryBlock), .sizeFree = sizeBytes }, .next = nullptr };

#ifdef DEV_LIVE_STATS
    mThread = std::thread(&DebugMemoryConsole::Init);
#endif // DEV_LIVE_STATS.
}

MemoryManager::~MemoryManager()
{
    free(mMemoryBlock);
}

void MemoryManager::Init(const uint32_t sizeBytes)
{
#ifdef DEV_CONFIGURATION
    if (mInstance)
    {
        MEM_LOG_ERROR("Trying to re-create memory manager when it already exists.");
        return;
    }
#endif // DEV_CONFIGURATION.

    mInstance = new MemoryManager(sizeBytes);
    MEM_LOG_DEBUG("Memory managed successfully initialized.");
}

void MemoryManager::Destroy()
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

    delete mInstance;
    mInstance = nullptr;
    MEM_LOG_DEBUG("Memory managed successfully destroyed.");
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
