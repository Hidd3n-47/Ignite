#pragma once

#include "Defines.h"
#include "Node.h"

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

    template <typename T>
    void Delete(T* free);

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

    //todo can these be debug only.
    uint64_t mSize;
    uint64_t mAllocated{};

    static constexpr uint32_t METADATA_PADDING{ 4 };

    Node* FindBestNodeForAllocation(const uint32_t size);
};

template <typename T>
T* MemoryManager::New()
{
    const uint32_t size = sizeof(T);
    Node* allocationNode = FindBestNodeForAllocation(size);

    *reinterpret_cast<uint32_t*>(allocationNode->start) = size;

    const uint64_t allocatedSize = size + METADATA_PADDING;
    mAllocated += allocatedSize;

    allocationNode->start += allocatedSize;
    allocationNode->size  -= allocatedSize;

    void* typeAddress = allocationNode->start + size;

    DEBUG(SetMemoryBlockDebug(DebugMemoryHexValues::NEWLY_ALLOCATED, typeAddress, size));

    return static_cast<T*>(typeAddress);
}

template <typename T>
void MemoryManager::Delete(T* free)
{
    
}

} // Namespace ignite::mem;