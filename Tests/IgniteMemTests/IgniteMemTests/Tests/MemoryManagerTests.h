#pragma once

#include <IgniteMem/Core/MemoryManager.h>

#include "Core/Defines.h"

class MemoryManagerTests
{
public:
    // Tests are only valid in Dev or Dev_LiveStats as external functions are only included in Dev configuration.
#ifdef DEV_CONFIGURATION
    constexpr static uint64_t MEMORY_MANAGER_SIZE = 128;
    explicit inline MemoryManagerTests(ignite::test::TestRegistry* testRegistry)
    {
        ignite::mem::MemoryManager::Init(MEMORY_MANAGER_SIZE);

        testRegistry->AddTestCase("MemoryManagerTests", "MemoryManagerInitializedCorrectly", MemoryManagerInitializedCorrectly);
        testRegistry->AddTestCase("MemoryManagerTests", "MemoryManagerInitializedUnallocatedMemoryInDebugMode", MemoryManagerInitializedUnallocatedMemoryInDebugMode);
        testRegistry->AddTestCase("MemoryManagerTests", "MemoryManagerInitializedFreedMemoryInDebugMode", MemoryManagerInitializedFreedMemoryInDebugMode);
        testRegistry->AddTestCase("MemoryManagerTests", "MemoryAddedToHeapInDebugModeWhenReservedBlockFull", MemoryAddedToHeapInDebugModeWhenReservedBlockFull);
        testRegistry->AddTestCase("MemoryManagerTests", "FragmentNodeAddedToHeapWhenReservedBlockFull", FragmentNodeAddedToHeapWhenReservedBlockFull);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocatedCorrectSizeInt", AllocatedCorrectSizeInt);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocatedCorrectSizeInt64", AllocatedCorrectSizeInt64);
        testRegistry->AddTestCase("MemoryManagerTests", "Allocated3Int64AndFreedReverseOrder", Allocated3Int64AndFreedReverseOrder);
        testRegistry->AddTestCase("MemoryManagerTests", "Allocated3Int64AndFreedInSameOrder", Allocated3Int64AndFreedInSameOrder);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocatedDifferentSizeAndFreedInDifferentOrderTest1", AllocatedDifferentSizeAndFreedInDifferentOrderTest1);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocatesInFirstFreeSpace", AllocatesInFirstFreeSpace);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocatesInFirstFreeSpaceAndThenInNextFreeBlock", AllocatesInFirstFreeSpaceAndThenInNextFreeBlock);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocateLargerSkipsSmallGap", AllocateLargerSkipsSmallGap);
        testRegistry->AddTestCase("MemoryManagerTests", "AllocateFullFreeCollapsesAndInsertInMiddle", AllocateFullFreeCollapsesAndInsertInMiddle);
        testRegistry->AddTestCase("MemoryManagerTests", "WhenFullyAllocatedOnlyHaveRootNodeAndNoneOthers", WhenFullyAllocatedOnlyHaveRootNodeAndNoneOthers);
        testRegistry->AddTestCase("LinkedListNodeTest", "StartingNodeCollapsesWhenFullAndAnotherNodeExists", StartingNodeCollapsesWhenFullAndAnotherNodeExists);
        testRegistry->AddTestCase("LinkedListNodeTest", "StartingNodeCollapsesWhenFullAndNoOtherNodeExists", StartingNodeCollapsesWhenFullAndNoOtherNodeExists);
        testRegistry->AddTestCase("LinkedListNodeTest", "MiddleNodeCollapsesAndListReLinks", MiddleNodeCollapsesAndListReLinks);
        testRegistry->AddTestCase("LinkedListNodeTest", "EndNodeCollapsesWhenThereIsALeftNode", EndNodeCollapsesWhenThereIsALeftNode);
        testRegistry->AddTestCase("LinkedListNodeTest", "MiddleBlockAddedThenCollapsedThenAdded", MiddleBlockAddedThenCollapsedThenAdded);
        testRegistry->AddTestCase("Construction", "AllocatingCallsConstructor", AllocatingCallsConstructor);
        testRegistry->AddTestCase("Construction", "DeallocatingCallsDestructor", DeallocatingCallsDestructor);
        testRegistry->AddTestCase("Construction", "ConstructorCalledWithParameters", ConstructorCalledWithParameters);

        testRegistry->SetOnTestFailedCallback(CreateMemoryManager);
    }

    static void CreateMemoryManager()
    {
        ignite::mem::MemoryManager::Destroy();

        ignite::mem::MemoryManager::Init(MEMORY_MANAGER_SIZE);
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

    static std::optional<std::string> MemoryManagerInitializedCorrectly()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        DEBUG(if (manager->GetSize()      != MEMORY_MANAGER_SIZE) return { "Memory manager has incorrect size." });
        DEBUG(if (manager->GetAllocated() != 0)                   return { "Memory manager has incorrect allocation size at initialization." });
        DEBUG(if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager has incorrect size free at initialization." });

        return {};
    }

    static std::optional<std::string> MemoryManagerInitializedUnallocatedMemoryInDebugMode()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        const uint64_t startingByte = *(uint64_t*)manager->GetStartOfMemoryBlock();

        uint64_t unallocatedByteCode;
        memset(&unallocatedByteCode, static_cast<int>(ignite::mem::DebugMemoryHexValues::UNALLOCATED), sizeof(uint64_t));

        if (startingByte != unallocatedByteCode) return { "Memory manager has not initialized memory to UNALLOCATED code in dev mode." };

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

    static std::optional<std::string> MemoryAddedToHeapInDebugModeWhenReservedBlockFull()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        constexpr uint32_t arraySize = 8;
        Bytes16* bytes[arraySize];

        for (Bytes16*& b : bytes)
        {
            b = manager->New<Bytes16>();
            DEV_PAUSE();
        }

        const uint32_t* integer = manager->New<uint32_t>();

        if ((std::byte*)integer > (std::byte*)manager->GetStartOfMemoryBlock() && (std::byte*)integer < (std::byte*)manager->GetStartOfMemoryBlock() + manager->GetSize()) return { "Allocation when memory full is within reserved memory block." };

        manager->Delete(integer);
        DEV_PAUSE();

        for (Bytes16*& b : bytes)
        {
            manager->Delete(b);
            DEV_PAUSE();
        }

        return {};
    }

    static std::optional<std::string> FragmentNodeAddedToHeapWhenReservedBlockFull()
    {
        ignite::mem::MemoryManager::Destroy();
        ignite::mem::MemoryManager::Init(660);

        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        constexpr uint32_t arraySize = 130;
        char* ints[arraySize];

        for (char*& b : ints)
        {
            b = manager->New<char>();
        }
        DEV_PAUSE();


        for (uint32_t i{ 0 }; i < arraySize; i += 2)
        {
            manager->Delete(ints[i]);
        }
        DEV_PAUSE();

        if (!manager->GetListStack().empty()) return { "The stack of free list indices has not been exhausted." };

        for (uint32_t i{ 1 }; i < arraySize; i += 2)
        {
            manager->Delete(ints[i]);
        }
        DEV_PAUSE();

        ignite::mem::MemoryManager::Destroy();
        ignite::mem::MemoryManager::Init(MEMORY_MANAGER_SIZE);

        return {};
    }

    static std::optional<std::string> AllocatedCorrectSizeInt()
    {
        DEV_PAUSE();

        const uint64_t allocSize = sizeof(int) + ignite::mem::MemoryManager::GetMetadataPadding();

        const int*  integer         = ignite::mem::MemoryManager::Instance()->New<int>();
        const void* startingAddress = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(integer) - ignite::mem::MemoryManager::GetMetadataPadding());

        if (startingAddress != ignite::mem::MemoryManager::Instance()->GetStartOfMemoryBlock())        return { "Memory manager first allocation is not the starting address of the memory block." };
        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

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
        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

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

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

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

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

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

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

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

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

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

    static std::optional<std::string> AllocatesInFirstFreeSpace()
    {
        DEV_PAUSE();

        const uint32_t* integer1 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();
        DEV_PAUSE();
        const uint32_t* integer2 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();

        const uint64_t allocSize = 2 * sizeof(uint32_t) + 2 * ignite::mem::MemoryManager::GetMetadataPadding();

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

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

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

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

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

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

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

        DEV_PAUSE();

        constexpr uint32_t freeOrder[] = { 1, 3, 5 };
        for (const uint32_t index : freeOrder)
        {
            ignite::mem::MemoryManager::Instance()->Delete(bytes[index]);
            DEV_PAUSE();
        }

        uint32_t fragmentsBefore = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        ignite::mem::MemoryManager::Instance()->Delete(bytes[2]);
        uint32_t fragmentsAfter = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        if (fragmentsBefore != fragmentsAfter + 1) return { "Freeing should have resulted in a collapse of fragments" };
        DEV_PAUSE();

        fragmentsBefore = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        ignite::mem::MemoryManager::Instance()->Delete(bytes[4]);
        fragmentsAfter = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        if (fragmentsBefore != fragmentsAfter + 1) return { "Freeing should have resulted in a collapse of fragments" };
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

    static std::optional<std::string> WhenFullyAllocatedOnlyHaveRootNodeAndNoneOthers()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        constexpr uint32_t arraySize = 8;
        Bytes16* bytes[arraySize];

        for (Bytes16*& b : bytes)
        {
            b = ignite::mem::MemoryManager::Instance()->New<Bytes16>();
            DEV_PAUSE();
        }

        const uint64_t allocSize = arraySize * (sizeof(Bytes16) + ignite::mem::MemoryManager::GetMetadataPadding());

        if (manager->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

        DEV_PAUSE();

        if (ignite::mem::MemoryManager::Instance()->GetNumberOfFragments() != 1) return { "Multiple fragments when there should only be one fully allocated block." };
        if (manager->GetStartingListNode()->value.sizeFree != 0)                 return { "The block still has free space when all space should be allocated." };
        DEV_PAUSE();

        for (const Bytes16* b : bytes)
        {
            manager->Delete(b);
            DEV_PAUSE();
        }

        if (manager->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    static std::optional<std::string> StartingNodeCollapsesWhenFullAndAnotherNodeExists()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        constexpr uint32_t arraySize = 4;
        Bytes16* bytes[arraySize];

        for (Bytes16*& b : bytes)
        {
            b = ignite::mem::MemoryManager::Instance()->New<Bytes16>();
            DEV_PAUSE();
        }

        const uint64_t allocSize = arraySize * (sizeof(Bytes16) + ignite::mem::MemoryManager::GetMetadataPadding());
        if (manager->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

        DEV_PAUSE();
        manager->Delete(bytes[0]);
        DEV_PAUSE();

        manager->Delete(bytes[2]);

        const uint32_t   fragmentCount = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        const std::byte* blockStart    = manager->GetStartingListNode()->value.address;
        const std::byte* memoryStart   = (std::byte*)manager->GetStartOfMemoryBlock();
        if (fragmentCount != 3)   return { "Incorrect number of fragments for given situation." };
        if (blockStart != memoryStart) return { "First memory block should be the start of the memory block." };
        DEV_PAUSE();

        const Bytes16* reallocated = ignite::mem::MemoryManager::Instance()->New<Bytes16>();

        const uint32_t   newFragmentCount = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        const std::byte* newBlockStart    = manager->GetStartingListNode()->value.address;
        if (newFragmentCount != 2)        return { "Incorrect number of fragments for given situation." };
        if (newBlockStart == memoryStart) return { "First memory block should not be the start of the memory block." };
        DEV_PAUSE();

        manager->Delete(bytes[1]);
        DEV_PAUSE();
        manager->Delete(bytes[3]);
        DEV_PAUSE();
        manager->Delete(reallocated);
        DEV_PAUSE();

        if (manager->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    static std::optional<std::string> StartingNodeCollapsesWhenFullAndNoOtherNodeExists()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        constexpr uint32_t arraySize = 8;
        Bytes16* bytes[arraySize];

        for (Bytes16*& b : bytes)
        {
            b = ignite::mem::MemoryManager::Instance()->New<Bytes16>();
            DEV_PAUSE();
        }

        const uint64_t allocSize = arraySize * (sizeof(Bytes16) + ignite::mem::MemoryManager::GetMetadataPadding());
        if (manager->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

        DEV_PAUSE();
        manager->Delete(bytes[0]);
        DEV_PAUSE();

        bytes[0] = ignite::mem::MemoryManager::Instance()->New<Bytes16>();

        const uint32_t   fragmentCount = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        const std::byte* blockStart    = manager->GetStartingListNode()->value.address;
        const std::byte* memoryStart   = (std::byte*)manager->GetStartOfMemoryBlock();
        if (fragmentCount != 1)                                  return { "Should only have a single fragment." };
        if (blockStart != memoryStart + 16)                      return { "First memory block should be one allocation of Bytes16 off of the starting block address." };
        if (manager->GetStartingListNode()->value.sizeFree != 0) return { "First memory block should have no space free to allocate." };
        /*
         * Note the block address should be one Bytes16 off the starting block because of the final allocation.
         * The only memory block pre-allocation would start at block base and be the size of 16.
         * Once we allocate, the allocation would reduce the size (so 0) and increment the starting address by allocation size.
         * Therefore, it would be base + sizeof(Bytes16) [+padding] = base + 16.
         */
        DEV_PAUSE();

        for (Bytes16*& b : bytes)
        {
            manager->Delete(b);
            DEV_PAUSE();
        }

        if (manager->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    static std::optional<std::string> MiddleNodeCollapsesAndListReLinks()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        constexpr uint32_t arraySize = 3;
        Bytes16* bytes[arraySize];

        const uint32_t* integer1 = ignite::mem::MemoryManager::Instance()->New<uint32_t>();

        for (Bytes16*& b : bytes)
        {
            b = ignite::mem::MemoryManager::Instance()->New<Bytes16>();
            DEV_PAUSE();
        }

        const uint64_t allocSize = arraySize * sizeof(Bytes16) + sizeof(uint32_t) + (arraySize + 1) * ignite::mem::MemoryManager::GetMetadataPadding();
        if (manager->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

        DEV_PAUSE();
        manager->Delete(integer1);
        DEV_PAUSE();
        manager->Delete(bytes[1]);

        const uint32_t   fragmentCount = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        const std::byte* blockStart  = manager->GetStartingListNode()->value.address;
        const std::byte* memoryStart = (std::byte*)manager->GetStartOfMemoryBlock();
        if (fragmentCount != 3)        return { "Incorrect number of fragments for given situation." };
        if (blockStart != memoryStart) return { "First memory block should be the start of the memory block." };
        DEV_PAUSE();

        const Bytes16* reallocated = ignite::mem::MemoryManager::Instance()->New<Bytes16>();

        const uint32_t   newFragmentCount          = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        const std::byte* newBlockStart             = manager->GetStartingListNode()->value.address;
        const std::byte* nextBlockStart            = manager->GetStartingListNode()->next->value.address;
        const std::byte* addressOfEndOfAllocations = (std::byte*)bytes[2] + sizeof(Bytes16);
        if (newFragmentCount != 2)                       return { "Incorrect number of fragments for given situation." };
        if (newBlockStart  != memoryStart)               return { "First memory block should be the start of the memory block." };
        if (reallocated    != bytes[1])                  return { "Reallocation should be in the middle." };
        if (nextBlockStart != addressOfEndOfAllocations) return { "Final block should start at the end of previous allocations." };
        DEV_PAUSE();

        manager->Delete(bytes[0]);
        DEV_PAUSE();
        manager->Delete(bytes[2]);
        DEV_PAUSE();
        manager->Delete(reallocated);
        DEV_PAUSE();

        if (manager->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    static std::optional<std::string> EndNodeCollapsesWhenThereIsALeftNode()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        constexpr uint32_t arraySize = 7;
        Bytes16* bytes[arraySize];

        uint32_t* integer1 = manager->New<uint32_t>();
        uint32_t* integer2 = manager->New<uint32_t>();

        for (Bytes16*& b : bytes)
        {
            b = ignite::mem::MemoryManager::Instance()->New<Bytes16>();
            DEV_PAUSE();
        }

        const uint64_t allocSize = (arraySize + 1) * (sizeof(Bytes16) + ignite::mem::MemoryManager::GetMetadataPadding());
        if (manager->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

        DEV_PAUSE();
        manager->Delete(integer1);
        DEV_PAUSE();
        manager->Delete(bytes[arraySize - 1]);

        const uint32_t   fragmentCount = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        const std::byte* blockStart    = manager->GetStartingListNode()->value.address;
        const std::byte* memoryStart   = (std::byte*)manager->GetStartOfMemoryBlock();
        if (fragmentCount != 2)                              return { "Should have two memory fragments after freeing." };
        if (blockStart != memoryStart)                       return { "First memory block should be the base address of memory block." };
        if (manager->GetStartingListNode()->next == nullptr) return { "Starting memory block should point to the end memory block." };

        DEV_PAUSE();
        bytes[arraySize - 1] = ignite::mem::MemoryManager::Instance()->New<Bytes16>();

        const uint32_t newFragmentCount = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        if (newFragmentCount != 1)                           return { "Last memory block should have been collapsed and removed due to not having space to allocate." };
        if (blockStart != memoryStart)                       return { "First memory block should be the base address of memory block." };
        if (manager->GetStartingListNode()->next != nullptr) return { "Starting memory block remove link to next block as it collapsed." };
        DEV_PAUSE();

        for (Bytes16*& b : bytes)
        {
            manager->Delete(b);
            DEV_PAUSE();
        }
        manager->Delete(integer2);

        if (manager->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    static std::optional<std::string> MiddleBlockAddedThenCollapsedThenAdded()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        constexpr uint32_t arraySize = 7;
        Bytes16* bytes[arraySize];

        uint32_t* integer1 = manager->New<uint32_t>();
        uint32_t* integer2 = manager->New<uint32_t>();

        for (Bytes16*& b : bytes)
        {
            b = ignite::mem::MemoryManager::Instance()->New<Bytes16>();
            DEV_PAUSE();
        }

        const uint64_t allocSize = (arraySize + 1) * (sizeof(Bytes16) + ignite::mem::MemoryManager::GetMetadataPadding());
        if (manager->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

        DEV_PAUSE();
        manager->Delete(integer1);
        DEV_PAUSE();
        manager->Delete(bytes[3]);
        DEV_PAUSE();
        manager->Delete(bytes[arraySize - 1]);
        bytes[arraySize - 1] = nullptr;

        const std::byte* blockStart    = manager->GetStartingListNode()->value.address;
        std::byte*       memoryStart   = (std::byte*)manager->GetStartOfMemoryBlock();
        uint32_t         fragmentCount = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        if (fragmentCount != 3)        return { "Should have three memory fragments after freeing." };
        if (blockStart != memoryStart) return { "First memory block should be the base address of memory block." };

        DEV_PAUSE();
        bytes[3] = ignite::mem::MemoryManager::Instance()->New<Bytes16>();

        fragmentCount = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        memoryStart   = (std::byte*)manager->GetStartOfMemoryBlock();
        if (fragmentCount != 2)        return { "Middle node should collapse leaving two." };
        if (blockStart != memoryStart) return { "First memory block should be the base address of memory block." };
        DEV_PAUSE();

        manager->Delete(bytes[3]);
        bytes[3] = nullptr;

        fragmentCount = ignite::mem::MemoryManager::Instance()->GetNumberOfFragments();
        memoryStart   = (std::byte*)manager->GetStartOfMemoryBlock();
        if (fragmentCount != 3)        return { "Middle freed, so should have 3 blocks again." };
        if (blockStart != memoryStart) return { "First memory block should be the base address of memory block." };
        DEV_PAUSE();

        for (Bytes16*& b : bytes)
        {
            if (b)
            {
                manager->Delete(b);
                DEV_PAUSE();
            }
        }
        manager->Delete(integer2);

        if (manager->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE)  return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    struct Constructor
    {
        Constructor()
        {
            a = solution;
        }
        uint32_t a{0};

        static constexpr uint32_t solution{ 255 };
    };

    static std::optional<std::string> AllocatingCallsConstructor()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        Constructor* instance = manager->New<Constructor>();
        if (instance->a != Constructor::solution) return { "Failed to construct instance after allocating." };

        const uint64_t allocSize = sizeof(Constructor) + ignite::mem::MemoryManager::GetMetadataPadding();
        if (manager->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

        DEV_PAUSE();
        manager->Delete(instance);

        if (ignite::mem::MemoryManager::Instance()->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (ignite::mem::MemoryManager::Instance()->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    struct Destructor
    {
        ~Destructor()
        {
            a = solution;
        }
        inline static uint32_t a { 0 };

        static constexpr uint32_t solution{ 255 };
    };

    static std::optional<std::string> DeallocatingCallsDestructor()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        Destructor* instance = manager->New<Destructor>();

        const uint64_t allocSize = sizeof(Destructor) + ignite::mem::MemoryManager::GetMetadataPadding();
        if (manager->GetAllocated() != allocSize)                       return { "Memory manager allocated more than the size of the allocation." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE - allocSize) return { "Memory manager's free space was reduced by more than the size of allocation." };

        DEV_PAUSE();

        manager->Delete(instance);
        if (Destructor::a != Destructor::solution) return { "Failed to destructed instance after deallocating." };

        if (manager->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }

    struct Vec2
    {
        Vec2(const float x, const float y) : x(x), y(y) { }

        float x = 0.0f, y = 0.0f;

        constexpr static float solution = 3.14159f;
    };

    static std::optional<std::string> ConstructorCalledWithParameters()
    {
        ignite::mem::MemoryManager* manager = ignite::mem::MemoryManager::Instance();

        Vec2* instance = manager->New<Vec2>(Vec2::solution, -Vec2::solution);

        const uint64_t allocSize = sizeof(Vec2) + ignite::mem::MemoryManager::GetMetadataPadding();
        if (manager->GetAllocated()  != allocSize)                                        return { "Memory manager allocated more than the size of the allocation." };
        if (manager->GetSizeFree()   != MEMORY_MANAGER_SIZE - allocSize)                  return { "Memory manager's free space was reduced by more than size of allocation" };
        if (!instance || instance->x != Vec2::solution || instance->y != -Vec2::solution) return { "Instances' parameters were not forwarded to constructed and constructed with correct values." };

        DEV_PAUSE();

        manager->Delete(instance);

        if (manager->GetAllocated() != 0)                   return { "Memory manager still has allocated values even after freeing." };
        if (manager->GetSizeFree()  != MEMORY_MANAGER_SIZE) return { "Memory manager's free space was not reset after freeing." };

        DEV_PAUSE();

        return {};
    }
#else  // DEV_CONFIGURATION.
explicit inline MemoryManagerTests(ignite::test::TestRegistry* testRegistry) {}
#endif // !DEV_CONFIGURATION.
};
