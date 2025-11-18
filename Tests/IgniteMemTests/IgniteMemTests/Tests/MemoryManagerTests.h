#pragma once

#include <iostream>
#include <IgniteMem/Core/MemoryManager.h>

#include "Core/Defines.h"

class MemoryManagerTests
{
public:
    constexpr static uint64_t MEMORY_MANAGER_SIZE = 128;
    explicit inline MemoryManagerTests(ignite::test::TestRegistry* testRegistry)
    {
        ignite::mem::MemoryManager::Init(MEMORY_MANAGER_SIZE);

        testRegistry->AddTestCase("MemoryManagerTests", "MemoryManagerInitializedCorrectly", MemoryManagerInitializedCorrectly);
        DEBUG(testRegistry->AddTestCase("MemoryManagerTests", "MemoryManagerInitializedUnallocatedMemoryInDebugMode", MemoryManagerInitializedUnallocatedMemoryInDebugMode));
        DEBUG(testRegistry->AddTestCase("MemoryManagerTests", "MemoryManagerInitializedAllocatedMemoryInDebugMode", MemoryManagerInitializedAllocatedMemoryInDebugMode));
        DEBUG(testRegistry->AddTestCase("MemoryManagerTests", "MemoryManagerInitializedFreedMemoryInDebugMode", MemoryManagerInitializedFreedMemoryInDebugMode));
        testRegistry->AddTestCase("MemoryManagerTests", "AllocatedCorrectSizeInt", AllocatedCorrectSizeInt);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocatedCorrectSizeInt64", AllocatedCorrectSizeInt64);
        testRegistry->AddTestCase("MemoryManagerTests", "Allocated3Int64AndFreedReverseOrder", Allocated3Int64AndFreedReverseOrder);
        testRegistry->AddTestCase("MemoryManagerTests", "Allocated3Int64AndFreedInSameOrder", Allocated3Int64AndFreedInSameOrder);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocatedDifferentSizeAndFreedInDifferentOrderTest1", AllocatedDifferentSizeAndFreedInDifferentOrderTest1);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocatesInFirstFreeSpace", AllocatesInFirstFreeSpace);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocatesInFirstFreeSpaceAndThenInNextFreeBlock", AllocatesInFirstFreeSpaceAndThenInNextFreeBlock);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocateLargerSkipsSmallGap", AllocateLargerSkipsSmallGap);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocateFullFreeCollapsesAndInsertInMiddle", AllocateFullFreeCollapsesAndInsertInMiddle);
    }

    static std::optional<std::string> MemoryManagerInitializedCorrectly()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        if (manager->GetSize()      != MEMORY_MANAGER_SIZE) return { "Memory manager has incorrect size." };
        if (manager->GetAllocated() != 0)                   return { "Memory manager has incorrect allocation size at initialization." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager has incorrect size free at initialization." };

        return {};
    }

#ifdef DEV_CONFIGURATION
    static std::optional<std::string> MemoryManagerInitializedUnallocatedMemoryInDebugMode()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        const uint64_t startingByte = *(uint64_t*)manager->GetStartOfMemoryBlock();

        uint64_t unallocatedByteCode;
        memset(&unallocatedByteCode, static_cast<int>(ignite::mem::DebugMemoryHexValues::UNALLOCATED), sizeof(uint64_t));

        if (startingByte != unallocatedByteCode) return { "Memory manager has not initialized memory to UNALLOCATED code in dev mode." };

        return {};
    }

    static std::optional<std::string> MemoryManagerInitializedAllocatedMemoryInDebugMode()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        const uint64_t* integer = manager->New<uint64_t>();

        uint64_t allocatedByteCode;
        memset(&allocatedByteCode, static_cast<int>(ignite::mem::DebugMemoryHexValues::NEWLY_ALLOCATED), sizeof(uint64_t));

        if (*integer != allocatedByteCode) return { "Memory manager has not initialized allocated memory to NEWLY_ALLOCATED code in dev mode." };

        manager->Delete(integer);

        return {};
    }

    static std::optional<std::string> MemoryManagerInitializedFreedMemoryInDebugMode()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        const uint64_t* integer = manager->New<uint64_t>();
        manager->Delete(integer);

        uint64_t allocatedByteCode;
        memset(&allocatedByteCode, static_cast<int>(ignite::mem::DebugMemoryHexValues::FREED), sizeof(uint64_t));

        if (*integer != allocatedByteCode) return { "Memory manager has not initialized freed memory to FREED code in dev mode." };

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

        ignite::mem::MemoryManager::Instance()->Delete(integer);

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    static std::optional<std::string> AllocatedCorrectSizeInt64()
    {
        DEV_PAUSE();

        const uint64_t* integer = ignite::mem::MemoryManager::Instance()->New<uint64_t>();

        const uint64_t allocSize    = sizeof(uint64_t) + ignite::mem::MemoryManager::GetMetadataPadding();
        const void* startingAddress = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(integer) - ignite::mem::MemoryManager::GetMetadataPadding());

        if (startingAddress != ignite::mem::MemoryManager::Instance()->GetStartOfMemoryBlock())        return { "Memory manager first allocation is not the starting address of the memory block." };
        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the int." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of int." };

        DEV_PAUSE();

        ignite::mem::MemoryManager::Instance()->Delete(integer);

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    static std::optional<std::string> Allocated3Int64AndFreedReverseOrder()
    {
        DEV_PAUSE();

        const uint64_t* integer1 = ignite::mem::MemoryManager::Instance()->New<uint64_t>();
        DEV_PAUSE();
        const uint64_t* integer2 = ignite::mem::MemoryManager::Instance()->New<uint64_t>();
        DEV_PAUSE();
        const uint64_t* integer3 = ignite::mem::MemoryManager::Instance()->New<uint64_t>();

        const uint64_t allocSize = 3 * (sizeof(uint64_t) + ignite::mem::MemoryManager::GetMetadataPadding());

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the int." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of int." };

        DEV_PAUSE();

        ignite::mem::MemoryManager::Instance()->Delete(integer3);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer2);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer1);

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    static std::optional<std::string> Allocated3Int64AndFreedInSameOrder()
    {
        DEV_PAUSE();

        const uint64_t* integer1 = ignite::mem::MemoryManager::Instance()->New<uint64_t>();
        DEV_PAUSE();
        const uint64_t* integer2 = ignite::mem::MemoryManager::Instance()->New<uint64_t>();
        DEV_PAUSE();
        const uint64_t* integer3 = ignite::mem::MemoryManager::Instance()->New<uint64_t>();

        const uint64_t allocSize = 3 * (sizeof(uint64_t) + ignite::mem::MemoryManager::GetMetadataPadding());

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the int." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of int." };

        DEV_PAUSE();

        ignite::mem::MemoryManager::Instance()->Delete(integer1);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer2);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer3);

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    static std::optional<std::string> AllocatedDifferentSizeAndFreedInDifferentOrderTest1()
    {
        DEV_PAUSE();

        const uint32_t* integer1 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();
        DEV_PAUSE();
        const uint32_t* integer2 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();
        DEV_PAUSE();
        const uint64_t* integer3 = ignite::mem::MemoryManager::Instance()->New<uint64_t>();
        DEV_PAUSE();
        const uint32_t* integer4 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();

        const uint64_t allocSize = 3 * sizeof(uint32_t) + sizeof(uint64_t) + 4 * ignite::mem::MemoryManager::GetMetadataPadding();

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the int." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of int." };

        DEV_PAUSE();

        ignite::mem::MemoryManager::Instance()->Delete(integer1);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer3);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer2);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer4);

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    static std::optional<std::string> FreeingMemoryAdjacentToFreeBlocksMergeToSingleBlock()
    {
        DEV_PAUSE();

        const uint32_t* integer1 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();
        DEV_PAUSE();
        const uint32_t* integer2 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();
        DEV_PAUSE();
        const uint64_t* integer3 = ignite::mem::MemoryManager::Instance()->New<uint64_t>();
        DEV_PAUSE();
        const uint32_t* integer4 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();

        const uint64_t allocSize = 3 * sizeof(uint32_t) + sizeof(uint64_t) + 4 * ignite::mem::MemoryManager::GetMetadataPadding();

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the int." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree() != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of int." };

        DEV_PAUSE();

        ignite::mem::MemoryManager::Instance()->Delete(integer1);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer3);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer2);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer4);

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree() != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    static std::optional<std::string> AllocatesInFirstFreeSpace()
    {
        DEV_PAUSE();

        const uint32_t* integer1 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();
        DEV_PAUSE();
        const uint32_t* integer2 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();

        const uint64_t allocSize = 2 * sizeof(uint32_t) + 2 * ignite::mem::MemoryManager::GetMetadataPadding();

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the int." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of int." };

        DEV_PAUSE();

        ignite::mem::MemoryManager::Instance()->Delete(integer1);
        DEV_PAUSE();

        const uint32_t* integer3 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();
        DEV_PAUSE();
        if (integer1 != integer3) return { "Last allocated integer did not allocate in first free space." };

        ignite::mem::MemoryManager::Instance()->Delete(integer2);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer3);

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    static std::optional<std::string> AllocatesInFirstFreeSpaceAndThenInNextFreeBlock()
    {
        DEV_PAUSE();

        const uint32_t* integer1 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();
        DEV_PAUSE();
        const uint32_t* integer2 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();

        const uint32_t padding = ignite::mem::MemoryManager::GetMetadataPadding();
        const uint64_t allocSize = 2 * sizeof(uint32_t) + 2 * padding;

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the int." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of int." };

        DEV_PAUSE();

        ignite::mem::MemoryManager::Instance()->Delete(integer1);
        DEV_PAUSE();

        const uint32_t* integer3 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();
        if (integer1 != integer3) return { "Last allocated integer did not allocate in first free space." };
        DEV_PAUSE();

        const uint32_t* integer4 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();
        if ((std::byte*)integer4 != (std::byte*)integer2 + sizeof(uint32_t) + padding) return { "Allocation is not in next free block." };
        DEV_PAUSE();

        ignite::mem::MemoryManager::Instance()->Delete(integer2);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer3);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer4);

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    static std::optional<std::string> AllocateLargerSkipsSmallGap()
    {
        DEV_PAUSE();

        const uint32_t* integer1 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();
        DEV_PAUSE();
        const uint32_t* integer2 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();

        const uint64_t allocSize = 2 * sizeof(uint32_t) + 2 * ignite::mem::MemoryManager::GetMetadataPadding();

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the int." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of int." };

        DEV_PAUSE();

        ignite::mem::MemoryManager::Instance()->Delete(integer1);
        DEV_PAUSE();

        const uint64_t* integer3 = ignite::mem::MemoryManager::Instance()->New<uint64_t>();
        DEV_PAUSE();
        if ((void*)integer1 == (void*)integer3) return { "Allocated a larger size in first free spot which is too small in size." };

        ignite::mem::MemoryManager::Instance()->Delete(integer2);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(integer3);

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    // This is on purpose 4 bytes smaller as padding in memory manager is 4 bytes.
    struct Bytes16
    {
        char a[12];
    };

    // This is on purpose 4 bytes smaller as padding in memory manager is 4 bytes.
    struct Bytes80
    {
        char a[76];
    };

    static std::optional<std::string> AllocateFullFreeCollapsesAndInsertInMiddle()
    {
        constexpr uint32_t arraySize = 8;
        Bytes16* bytes[arraySize];

        for (Bytes16*& b : bytes)
        {
            b = ignite::mem::MemoryManager::Instance()->New<Bytes16>();
            DEV_PAUSE();
        }

        const uint64_t allocSize = arraySize * (sizeof(Bytes16) + ignite::mem::MemoryManager::GetMetadataPadding());

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the int." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of int." };

        DEV_PAUSE();

        constexpr uint32_t freeOrder[] = { 1, 3, 5 };
        for (const uint32_t index : freeOrder)
        {
            ignite::mem::MemoryManager::Instance()->Delete(bytes[index]);
            DEV_PAUSE();
        }

        //todo test collapse
        ignite::mem::MemoryManager::Instance()->Delete(bytes[2]);
        DEV_PAUSE();

        //todo test collapse
        ignite::mem::MemoryManager::Instance()->Delete(bytes[4]);
        DEV_PAUSE();

        const Bytes80* middleAllocation = ignite::mem::MemoryManager::Instance()->New<Bytes80>();
        DEV_PAUSE();

        ignite::mem::MemoryManager::Instance()->Delete(bytes[0]);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(bytes[7]);
        DEV_PAUSE();
        ignite::mem::MemoryManager::Instance()->Delete(bytes[6]);
        DEV_PAUSE();

        ignite::mem::MemoryManager::Instance()->Delete(middleAllocation);
        DEV_PAUSE();

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

};
