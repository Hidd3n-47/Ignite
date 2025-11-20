#include <IgniteMem/Core/MemoryManager.h>
#include <IgniteMem/Core/DebugMemoryConsole.h>

#include <algorithm>
#include <print>
#include <array>
#include <format>

#include "Timer.h"

int main(int, char**)
{
    ignite::mem::MemoryManager::Init(1'048'576);

    constexpr uint32_t size = 1'048'576 / 8 / 10;
    for (uint32_t q{ 0 }; q < 1; ++q)
    {
        std::println("----------------------------------------------------------\n\n");
        {
            std::array<float, size>  intAllocationTimes;
            std::array<uint32_t*, size> allocatedInts;
            for (uint32_t i{ 0 }; i < size; ++i)
            {
                ignite::bench::Timer timer;
                allocatedInts[i]      = ignite::mem::MemoryManager::Instance()->New<uint32_t>();
                intAllocationTimes[i] = static_cast<float>(timer.StopTimer());
            }

            uint64_t total = 0;
            uint64_t max   = 0;
            uint64_t min   = -1;
            for (const uint64_t time : intAllocationTimes)
            {
                max = std::max(time, max);
                min = std::min(time, min);

                total += time;
            }
            std::println("Allocations:\t{} times", size);
            std::println("Total Time :\t{} ns [{}  ms]", total, static_cast<double>(total) * 0.000001);
            std::println("Average Alloc Time: \t{} ns [{:.4f} ms]", total / size, static_cast<double>(total) / size * 0.000001);
            std::println("Best Alloc Time   : \t{} ns [{:.4f} ms]", min, static_cast<double>(min) * 0.000001);
            std::println("Worst Alloc Time  : \t{} ns [{:.4f} ms]", max, static_cast<double>(max) * 0.000001);
            std::println("----------------------------------------------------------");

            void* mem = intAllocationTimes.data();
            //ignite::mem::MemoryManager::Instance()->AddHistogram("Allocation Memory Manager", mem, size);

            std::array<uint64_t, size> intDeallocationTimes;
            for (uint32_t i{ 0 }; i < size; ++i)
            {
                ignite::bench::Timer timer;
                ignite::mem::MemoryManager::Instance()->Delete(allocatedInts[i]);
                intDeallocationTimes[i] = timer.StopTimer();
            }

            total = 0;
            max = 0;
            min = -1;
            for (const uint64_t time : intDeallocationTimes)
            {
                max = std::max(time, max);
                min = std::min(time, min);

                total += time;
            }
            std::println("Allocations:\t{} times", size);
            std::println("Total Time :\t{} ns [{}  ms]", total, static_cast<double>(total) * 0.000001);
            std::println("Average Dealloc Time: \t{} ns [{:.4f} ms]", total / size, static_cast<double>(total) / size * 0.000001);
            std::println("Best Dealloc Time   : \t{} ns [{:.4f} ms]", min, static_cast<double>(min) * 0.000001);
            std::println("Worst Dealloc Time  : \t{} ns [{:.4f} ms]", max, static_cast<double>(max) * 0.000001);
        }

        std::println("\n\n================ C++ New/Delete ================\n\n");

        {

            std::array<uint64_t, size>  intAllocationTimes;
            std::array<uint32_t*, size> allocatedInts;
            for (uint32_t i{ 0 }; i < size; ++i)
            {
                ignite::bench::Timer timer;
                allocatedInts[i] = new uint32_t();
                intAllocationTimes[i] = timer.StopTimer();
            }

            uint64_t total = 0;
            uint64_t max = 0;
            uint64_t min = -1;
            for (const uint64_t time : intAllocationTimes)
            {
                max = std::max(time, max);
                min = std::min(time, min);

                total += time;
            }
            std::println("Allocations:\t{} times", size);
            std::println("Total Time :\t{} ns [{}  ms]", total, static_cast<double>(total) * 0.000001);
            std::println("Average Alloc Time: \t{} ns [{:.4f} ms]", total / size, static_cast<double>(total) / size * 0.000001);
            std::println("Best Alloc Time   : \t{} ns [{:.4f} ms]", min, static_cast<double>(min) * 0.000001);
            std::println("Worst Alloc Time  : \t{} ns [{:.4f} ms]", max, static_cast<double>(max) * 0.000001);
            std::println("----------------------------------------------------------");

            std::array<uint64_t, size> intDeallocationTimes;
            for (uint32_t i{ 0 }; i < size; ++i)
            {
                ignite::bench::Timer timer;
                delete allocatedInts[i];
                intDeallocationTimes[i] = timer.StopTimer();
            }

            total = 0;
            max = 0;
            min = -1;
            for (const uint64_t time : intDeallocationTimes)
            {
                max = std::max(time, max);
                min = std::min(time, min);

                total += time;
            }
            std::println("Allocations:\t{} times", size);
            std::println("Total Time :\t{} ns [{}  ms]", total, static_cast<double>(total) * 0.000001);
            std::println("Average Dealloc Time: \t{} ns [{:.4f} ms]", total / size, static_cast<double>(total) / size * 0.000001);
            std::println("Best Dealloc Time   : \t{} ns [{:.4f} ms]", min, static_cast<double>(min) * 0.000001);
            std::println("Worst Dealloc Time  : \t{} ns [{:.4f} ms]", max, static_cast<double>(max) * 0.000001);

        }
        std::println("\n\n----------------------------------------------------------");
    }

    system("PAUSE");

    ignite::mem::MemoryManager::Destroy();
}