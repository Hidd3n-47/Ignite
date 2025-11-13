#pragma once

#include <iostream>
#include <IgniteMem/Core/MemoryManager.h>

#include "Core/Defines.h"

class MemoryManagerTests
{
public:
    constexpr static uint64_t MEMORY_MANAGER_SIZE = 1024;
    explicit inline MemoryManagerTests(ignite::test::TestRegistry* testRegistry)
    {
        ignite::mem::MemoryManager::Init(MEMORY_MANAGER_SIZE);

        testRegistry->AddTestCase("MemoryManagerTests", "MemoryManagerInitializedCorrectly", MemoryManagerInitializedCorrectly);
        DEBUG(testRegistry->AddTestCase("MemoryManagerTests", "MemoryManagerInitializedMemoryInDebugMode", MemoryManagerInitializedMemoryInDebugMode));
        testRegistry->AddTestCase("MemoryManagerTests", "AllocatedCorrectSizeInt", AllocatedCorrectSizeInt);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocatedCorrectSizeInt64", AllocatedCorrectSizeInt64);
    }

    static std::optional<std::string> MemoryManagerInitializedCorrectly()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        if (manager->GetSize() != MEMORY_MANAGER_SIZE)     return { "Memory manager has incorrect size." };
        if (manager->GetAllocated() != 0)                  return { "Memory manager has incorrect allocation size at initialization." };
        if (manager->GetSizeFree() != MEMORY_MANAGER_SIZE) return { "Memory manager has incorrect size free at initialization." };

        return {};
    }

#ifdef DEV_CONFIGURATION
    static std::optional<std::string> MemoryManagerInitializedMemoryInDebugMode()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        const uint64_t startingByte = *(uint64_t*)manager->GetStartOfMemoryBlock();

        uint64_t unallocatedByteCode;
        memset(&unallocatedByteCode, static_cast<int>(ignite::mem::DebugMemoryHexValues::UNALLOCATED), sizeof(uint64_t));

        if (startingByte != unallocatedByteCode) return { "Memory manager has not initialized memory to UNALLOCATED code in dev mode." };

        return {};
    }
#endif // DEV_CONFIGURATION.

    static std::optional<std::string> AllocatedCorrectSizeInt()
    {
        DEV_PAUSE();

        const uint64_t allocSize = sizeof(int) + ignite::mem::MemoryManager::GetMetadataPadding();

        const int*  integer         = ignite::mem::MemoryManager::Instance()->New<int>();
        const void* startingAddress = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(integer) - ignite::mem::MemoryManager::GetMetadataPadding());

        if (startingAddress != ignite::mem::MemoryManager::Instance()->GetStartOfMemoryBlock())        return { "Memory manager first allocation is not the starting address of the memory block." };
        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the int." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of int." };

        DEV_PAUSE();

        ignite::mem::MemoryManager::Instance()->Delete((void*)integer);

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }


    static std::optional<std::string> AllocatedCorrectSizeInt64()
    {
        const uint64_t* integer = ignite::mem::MemoryManager::Instance()->New<uint64_t>();

        DEV_PAUSE();

        const uint64_t allocSize = sizeof(uint64_t) + ignite::mem::MemoryManager::GetMetadataPadding();
        const void* startingAddress = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(integer) - ignite::mem::MemoryManager::GetMetadataPadding());

        if (startingAddress != ignite::mem::MemoryManager::Instance()->GetStartOfMemoryBlock())        return { "Memory manager first allocation is not the starting address of the memory block." };
        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the int." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of int." };

        DEV_PAUSE();

        ignite::mem::MemoryManager::Instance()->Delete((void*)integer);

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }
};
