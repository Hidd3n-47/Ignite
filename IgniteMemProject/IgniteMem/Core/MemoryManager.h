#pragma once

#include "Defines.h"
#include "Node.h"

#ifdef DEV_CONFIGURATION
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

class API MemoryManager
{
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

    void Delete(void* free);

#ifdef DEV_CONFIGURATION
    [[nodiscard]] inline Node*    GetRootNode()           const { return mRootNode; }
    [[nodiscard]] inline void*    GetStartOfMemoryBlock() const { return mMemoryBlock; }
    [[nodiscard]] inline uint64_t GetSize()               const { return mSize; }
    [[nodiscard]] inline uint64_t GetAllocated()          const { return mAllocated; }
    [[nodiscard]] inline uint64_t GetSizeFree()           const { return mSize - mAllocated; }

    static void SetMemoryBlockDebug(DebugMemoryHexValues value, void* memory, const uint64_t size);
    inline static uint32_t GetMetadataPadding() { return METADATA_PADDING; }
private:
    std::thread mThread;
public:
    friend class DebugMemoryConsole;
#endif // DEV_CONFIGURATION.

private:
    MemoryManager(const uint64_t sizeBytes);
    ~MemoryManager();

    static MemoryManager* mInstance;

    void* mMemoryBlock;

    Node* mRootNode      = nullptr;
    Node* mSmallestBlock = nullptr;
    Node* mLargestBlock  = nullptr;

    uint64_t mSize;
    uint64_t mAllocated{};

    static constexpr uint32_t METADATA_PADDING{ 4 };
};

template <typename T>
T* MemoryManager::New()
{
    // todo assert if the small node has any children nodes.

    const uint64_t size = sizeof(T);
    if (size <= mSmallestBlock->size)
    {
        *static_cast<uint32_t*>(mSmallestBlock->start) = size;

        const uint64_t allocatedSize = size + METADATA_PADDING;
        mAllocated += allocatedSize;
        mSmallestBlock->start = (uint8_t*)(mSmallestBlock->start) + allocatedSize;
        mSmallestBlock->size -= allocatedSize;
        // todo assert if the start is before the parents start.

        void* typeAddress = (uint8_t*)(mSmallestBlock->start) - size;

        DEBUG(SetMemoryBlockDebug(DebugMemoryHexValues::NEWLY_ALLOCATED, typeAddress, size));

        return (T*)(typeAddress);
    }

    // todo not enough space, need to traverse up the tree.
    return nullptr;
}

} // Namespace ignite::mem;