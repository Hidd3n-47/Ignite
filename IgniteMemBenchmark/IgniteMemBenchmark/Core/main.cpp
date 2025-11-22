#include <IgniteMem/Core/MemoryManager.h>

#include <print>
#include <format>

#include <fstream>
#include <set>
#include <unordered_set>

#include "Console.h"
#include "Timer.h"
#include "TestResultsManager.h"

constexpr uint32_t sizeBytes        = 12 * 1'024 * 1'024;
constexpr uint32_t sizeBytesRandom  = 5 * 1'024;

constexpr uint32_t numberOfTests = 20'000;
constexpr uint32_t numberOfTestsRun = 2'000;

static std::vector<void*>  allocations (numberOfTests);

static std::vector<double> allocStats;
static std::vector<double> allocStatsCpp;
static std::vector<double> deallocStats;
static std::vector<double> deallocStatsCpp;

static uint32_t padding{};
static uint64_t sizeFree{};

struct Component
{
    char a[64];
};

struct SmallerComponent
{
    char a[32];
};

struct LargerComponent
{
    char a[128];
    SmallerComponent b[10];
};

#define OUTPUT(X, ...)                 \
X << std::format(__VA_ARGS__) << '\n'; \
std::println(__VA_ARGS__)

static void PrintResults(std::ofstream& fileOutput, const double average, double min, double max, const bool deallocating = false)
{
    if (!deallocating)
    {
        OUTPUT(fileOutput, "Total Time           : \t{} ns [{:.4f} ms]", average * numberOfTests * numberOfTestsRun, average * numberOfTests * numberOfTestsRun * 0.000001);
        OUTPUT(fileOutput, "Average Alloc Time   : \t{} ns [{:.4f} ms]", average, average * 0.000001);
        OUTPUT(fileOutput, "Best Alloc Time      : \t{} ns [{:.4f} ms]", min, min * 0.000001);
        OUTPUT(fileOutput, "Worst Alloc Time     : \t{} ns [{:.4f} ms]", max, max * 0.000001);
    }
    else
    {
        OUTPUT(fileOutput, "Total Time           : \t{} ns [{:.4f} ms]", average * numberOfTests * numberOfTestsRun, average * numberOfTests * numberOfTestsRun * 0.000001);
        OUTPUT(fileOutput, "Average Dealloc Time : \t{} ns [{:.4f} ms]", average, average * 0.000001);
        OUTPUT(fileOutput, "Best Dealloc Time    : \t{} ns [{:.4f} ms]", min, min * 0.000001);
        OUTPUT(fileOutput, "Worst Dealloc Time   : \t{} ns [{:.4f} ms]", max, max * 0.000001);
    }

    OUTPUT(fileOutput, "----------------------------------------------------------");
}

template <typename T>
static void RunTestsOnSameType(const bool useMemoryManager = true)
{
    if (useMemoryManager)
    {
        const ignite::bench::Timer totalTimer;
        for (uint32_t i{ 0 }; i < numberOfTests; ++i)
        {
            allocations[i] = ignite::mem::MemoryManager::Instance()->New<T>();
        }
        allocStats.emplace_back(static_cast<double>(totalTimer.StopTimer()) / numberOfTests);
    }
    else
    {
        const ignite::bench::Timer totalTimer;
        for (uint32_t i{ 0 }; i < numberOfTests; ++i)
        {
            allocations[i] = new T{};
        }
        allocStatsCpp.emplace_back(static_cast<double>(totalTimer.StopTimer()) / numberOfTests);
    }

    if (useMemoryManager)
    {
        const ignite::bench::Timer totalTimer;
        for (uint32_t i{ 0 }; i < numberOfTests; ++i)
        {
            ignite::mem::MemoryManager::Instance()->Delete((T*)allocations[i]);
        }
        deallocStats.emplace_back(static_cast<double>(totalTimer.StopTimer()) / numberOfTests);
    }
    else
    {
        const ignite::bench::Timer totalTimer;
        for (uint32_t i{ 0 }; i < numberOfTests; ++i)
        {
            delete (T*)allocations[i];
        }
        deallocStatsCpp.emplace_back(static_cast<double>(totalTimer.StopTimer()) / numberOfTests);
    }

    if (useMemoryManager)
    {
        RunTestsOnSameType<T>(!useMemoryManager);
    }
}

template <typename T>
static bool AllocateSingle(void*& t, const bool memoryManagerAllocate = true)
{
    if (memoryManagerAllocate)
    {
        if (sizeof(uint32_t) + padding > sizeFree) return false;
        t = ignite::mem::MemoryManager::Instance()->New<T>();
    }
    else
    {
        t = new T{};
    }
    return true;
}

static void DeallocateSingle(const void* t, const bool memoryManagerAllocate = true)
{
    if (memoryManagerAllocate)
    {
        ignite::mem::MemoryManager::Instance()->Delete(t);
    }
    else
    {
        delete t;
    }
}

static void RunTestsOnRandomType(uint32_t seed, const bool useMemoryManager = true)
{
    srand(seed);

    std::vector<uint32_t> allocationIndices;
    allocationIndices.reserve(numberOfTests);

    padding  = ignite::mem::MemoryManager::Instance()->GetMetadataPadding();
    sizeFree = ignite::mem::MemoryManager::Instance()->GetSizeFree();
    uint32_t allocationIndex = 0;

    const ignite::bench::Timer totalTimer;
    for (uint32_t i{ 0 }; i < numberOfTests; ++i)
    {
        sizeFree = ignite::mem::MemoryManager::Instance()->GetSizeFree();

        const float allocatedPercent = static_cast<float>(sizeFree) / static_cast<float>(sizeBytesRandom);

        const uint32_t randNumber = rand() % 100;
        if (randNumber < static_cast<uint32_t>(allocatedPercent * 100.0f))
        {
            bool allocated = false;

            if (randNumber < 5)
            {
                if (AllocateSingle<uint32_t>(allocations[allocationIndex], useMemoryManager)) allocated = true;
            }
            else if (randNumber < 15)
            {
                if (AllocateSingle<SmallerComponent>(allocations[allocationIndex], useMemoryManager)) allocated = true;
            }
            else if (randNumber < 70)
            {
                if (AllocateSingle<Component>(allocations[allocationIndex], useMemoryManager)) allocated = true;
            }
            else
            {
                if (AllocateSingle<LargerComponent>(allocations[allocationIndex], useMemoryManager)) allocated = true;
            }

            if (allocated)
            {
                allocationIndices.emplace_back(allocationIndex);
                ++allocationIndex;
                continue;
            }
        }
        // Deallocate if we got the random number or if we didn't have space to allocate.
        if (!allocationIndices.empty())
        {
            const uint32_t index = randNumber % allocationIndices.size();
            DeallocateSingle(allocations[allocationIndices[index]], useMemoryManager);
            allocationIndices.erase(allocationIndices.begin() + index);
        }
    }

    while (!allocationIndices.empty())
    {
        const uint32_t randNumber = rand();
        const uint32_t index = randNumber % allocationIndices.size();
        DeallocateSingle(allocations[allocationIndices[index]], useMemoryManager);
        allocationIndices.erase(allocationIndices.begin() + index);
    }
    const uint64_t endTime = totalTimer.StopTimer();


    if (useMemoryManager)
    {
        allocStats.emplace_back(static_cast<double>(endTime) / numberOfTests);

        RunTestsOnRandomType(seed, !useMemoryManager);
    }
    else
    {
        allocStatsCpp.emplace_back(static_cast<double>(endTime) / numberOfTests);
    }
}

static void AddTestResult(const std::string& title, const std::optional<uint32_t> seed = {})
{
    ignite::bench::TestResult result{ .testName = title };
    result.memoryAllocTimes.resize(numberOfTestsRun);
    result.memoryAllocTimesCpp.resize(numberOfTestsRun);
    result.memoryDeallocTimes.resize(numberOfTestsRun);
    result.memoryDeallocTimesCpp.resize(numberOfTestsRun);

    memcpy(result.memoryAllocTimes.data(), allocStats.data(), allocStats.size() * sizeof(double));
    memcpy(result.memoryAllocTimesCpp.data(), allocStatsCpp.data(), allocStatsCpp.size() * sizeof(double));
    memcpy(result.memoryDeallocTimes.data(), deallocStats.data(), deallocStats.size() * sizeof(double));
    memcpy(result.memoryDeallocTimesCpp.data(), deallocStatsCpp.data(), deallocStatsCpp.size() * sizeof(double));

    ignite::bench::TestResultsManager::Instance()->AddTestResult(result);

    std::string fileName = "TestResults_" + title + ".result";
    std::ofstream fileOutput(fileName);
    if (fileOutput.fail())
    {
        std::println("Failed to open file, won't output test result to console or disk...");
        return;
    }

    OUTPUT(fileOutput, "================================================================================================");

    if (seed.has_value())
    {
        OUTPUT(fileOutput, "================ SEED: {} ================", seed.value());
    }

    OUTPUT(fileOutput, "================ Memory Manager ================");

    PrintResults(fileOutput, result.averageAlloc, result.minimumAllocTime, result.maximumAllocTime);
    PrintResults(fileOutput, result.averageDealloc, result.minimumDeallocTime, result.maximumDeallocTime, true);

    OUTPUT(fileOutput, "================ C++ New/Delete ================");

    PrintResults(fileOutput, result.averageAllocCpp, result.minimumAllocTimeCpp, result.maximumAllocTimeCpp);
    PrintResults(fileOutput, result.averageDeallocCpp, result.minimumDeallocTimeCpp, result.maximumDeallocTimeCpp, true);

    OUTPUT(fileOutput, "================================================================================================");

    fileOutput.close();
}

template <typename T>
static void RunTest()
{
    std::string title { typeid(T).name() };

    allocStats.reserve(numberOfTestsRun);
    allocStatsCpp.reserve(numberOfTestsRun);
    deallocStats.reserve(numberOfTestsRun);
    deallocStatsCpp.reserve(numberOfTestsRun);

    std::println("Starting test {}...", title);

    for (uint32_t i{ 0 }; i < numberOfTestsRun; ++i)
    {
        RunTestsOnSameType<T>();
    }

    AddTestResult(title);

    allocStats.clear();
    allocStatsCpp.clear();
    deallocStats.clear();
    deallocStatsCpp.clear();
}


static void RunTestRandom()
{
    srand(static_cast<uint32_t>(time(nullptr)));
    const uint32_t seed = rand();
    srand(seed);

    std::string title = std::format("RandomTestsSeed_{}", seed);

    allocStats.reserve(numberOfTestsRun);
    allocStatsCpp.reserve(numberOfTestsRun);
    deallocStats.reserve(numberOfTestsRun);
    deallocStatsCpp.reserve(numberOfTestsRun);

    std::println("Starting test {}...", title);

    for (uint32_t i{ 0 }; i < numberOfTestsRun; ++i)
    {
        RunTestsOnRandomType(rand());
    }

    AddTestResult(title, { seed });

    allocStats.clear();
    allocStatsCpp.clear();
    deallocStats.clear();
    deallocStatsCpp.clear();
}

int main(int, char**)
{
    ignite::mem::MemoryManager::Init(sizeBytes);

    RunTest<uint32_t>();
    RunTest<SmallerComponent>();
    RunTest<Component>();
    RunTest<LargerComponent>();

    ignite::mem::MemoryManager::Destroy();
    ignite::mem::MemoryManager::Init(sizeBytesRandom);

    RunTestRandom();

    ignite::mem::MemoryManager::Destroy();

    ignite::bench::Console::Init();
    ignite::bench::Console::Instance()->Run();

}