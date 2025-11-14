#include "MemoryManager.h"

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

    // todo make this a part of the reserved memory
    mRootNode = new Node{ .start = static_cast<std::byte*>(mMemoryBlock), .size = sizeBytes, .left = nullptr, .right = nullptr, .parent = nullptr };
    mSmallestBlock = mRootNode;
    mLargestBlock  = mRootNode;

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

void MemoryManager::SetMemoryBlockDebug(DebugMemoryHexValues value, void* memory, const uint64_t size)
{
    memset(memory, static_cast<int>(value), size);
}

void MemoryManager::TryUpdateSmallestBlock(Node* potentialSmallBlock)
{
    // If the new potential block is smaller, always set the smallest block to this.
    if (mSmallestBlock->size > potentialSmallBlock->size)
    {
        mSmallestBlock = potentialSmallBlock;
        return;
    }

    // If it's not smaller, the only other case we consider is if they are equal.
    if (mSmallestBlock->size != potentialSmallBlock->size)
    {
        return;
    }

    // If the blocks are equal size we should always point to the left most address.
    if (mSmallestBlock->start > potentialSmallBlock->start)
    {
        mSmallestBlock = potentialSmallBlock;
    }
}

void MemoryManager::TryUpdateLargestBlock(Node* potentialLargeBlock)
{
    // If the new potential block is larger, always set the larger block to this.
    if (mLargestBlock->size < potentialLargeBlock->size)
    {
        mLargestBlock = potentialLargeBlock;
        return;
    }

    // If it's not larger, the only other case we consider is if they are equal.
    if (mLargestBlock->size != potentialLargeBlock->size)
    {
        return;
    }

    // If the blocks are equal size we should always point to the left most address.
    if (mLargestBlock->start > potentialLargeBlock->start)
    {
        mLargestBlock = potentialLargeBlock;
    }
}

} // Namespace ignite::mem.