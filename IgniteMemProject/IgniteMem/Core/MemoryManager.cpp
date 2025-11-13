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
    mRootNode = new Node{ .start = mMemoryBlock, .size = sizeBytes, .left = nullptr, .right = nullptr, .parent = nullptr };
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

void MemoryManager::Delete(void* free)
{
    // todo make it take template type to prevent having to cast.

    Node* node = mRootNode;
    do
    {
        if (Node* comparingNode = reinterpret_cast<uintptr_t>(free) < reinterpret_cast<uintptr_t>(node->start) ? node->left : node->right; comparingNode)
        {
            node = comparingNode;
            continue;
        }

        void* baseAddress = (uint8_t*)free - METADATA_PADDING;

        const uint32_t size = *reinterpret_cast<uint32_t*>(baseAddress) + METADATA_PADDING;

        node->start = baseAddress;
        node->size += size;
        mAllocated -= size;

        DEBUG(SetMemoryBlockDebug(DebugMemoryHexValues::FREED, free, size - METADATA_PADDING));
        return;
    } 
    while (node->left || node->right);

    //todo assert if we get here.
    __debugbreak();
}

void MemoryManager::SetMemoryBlockDebug(DebugMemoryHexValues value, void* memory, const uint64_t size)
{
    memset(memory, static_cast<int>(value), size);
}

} // Namespace ignite::mem.