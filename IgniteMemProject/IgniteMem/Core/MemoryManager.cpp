#include "MemoryManager.h"

#include <cassert>
#include <string>

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

    //todo make this a part of the reserved memory
    //mRootNode = new Node{ .start = static_cast<std::byte*>(mMemoryBlock), .size = sizeBytes, .left = nullptr, .right = nullptr, .parent = nullptr };
    mStartingListNode = new ListNode{ .value = {.address = static_cast<std::byte*>(mMemoryBlock), .sizeFree = sizeBytes }, .next = nullptr};

#ifdef DEV_CONFIGURATION
    mThread = std::thread(&DebugMemoryConsole::Init);
#endif // DEV_CONFIGURATION.
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
        // todo assert/error as its already been init.
    }
#endif // DEV_CONFIGURATION.

    mInstance = new MemoryManager(sizeBytes);
}

void MemoryManager::Destroy()
{
#ifdef DEV_CONFIGURATION
    // Since the debug console is constructed on a different thread, it's possible to try close before the instance has been constructed.
    // Force stall until it's been created to ensure resources freed.
    while (!DebugMemoryConsole::Instance()) { }
    DebugMemoryConsole::Instance()->StopRunning();

    if (mInstance->mThread.joinable())
    {
        mInstance->mThread.join();
    }
#endif // DEV_CONFIGURATION.

    delete mInstance;
}

#ifdef DEV_CONFIGURATION
void MemoryManager::SetMemoryBlockDebug(DebugMemoryHexValues value, void* memory, const uint64_t size)
{
    memset(memory, static_cast<int>(value), size);
}
#endif // DEV_CONFIGURATION.

MemoryManager::ListNode* MemoryManager::FindAllocationListNode(const uint64_t size) const
{
    ListNode* node = mStartingListNode;
    while (node->value.sizeFree < size)
    {
        node = node->next;

        assert(node && "No valid size found to allocated.");
    }

    return node;
}

} // Namespace ignite::mem.