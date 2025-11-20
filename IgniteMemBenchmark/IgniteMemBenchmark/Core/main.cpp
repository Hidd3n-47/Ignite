#include <IgniteMem/Core/MemoryManager.h>

#include <print>
#include <format>

#include <fstream>

#include "Console.h"
#include "Timer.h"
#include "TestResultsManager.h"

constexpr uint32_t sizeBytes     = 500 * 1'024 * 1'024;

constexpr uint32_t numberOfTests = 20'000;
constexpr uint32_t numberOfTestsRun = 2'000;

static std::vector<void*>  allocations (numberOfTests);

static std::vector<double> allocStats;
static std::vector<double> allocStatsCpp;
static std::vector<double> deallocStats;
static std::vector<double> deallocStatsCpp;

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

static void AddTestResult(const std::string& title)
{
    ignite::bench::TestResult result{ .testName = title };
    result.memoryAllocTimes.resize(numberOfTestsRun);
    result.memoryAllocTimesCpp.resize(numberOfTestsRun);
    result.memoryDeallocTimes.resize(numberOfTestsRun);
    result.memoryDeallocTimesCpp.resize(numberOfTestsRun);

    memcpy(result.memoryAllocTimes.data(), allocStats.data(), allocStats.size() * sizeof(double));
    memcpy(result.memoryAllocTimesCpp.data(), allocStatsCpp.data(), allocStatsCpp.size() * sizeof(double));
    memcpy(result.memoryDeallocTimes.data(), allocStats.data(), deallocStats.size() * sizeof(double));
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
    OUTPUT(fileOutput, "================ Memory Manager ================");

    PrintResults(fileOutput, result.averageAlloc, result.minimumAllocTime, result.maximumAllocTime);
    PrintResults(fileOutput, result.averageAllocCpp, result.minimumAllocTimeCpp, result.maximumAllocTimeCpp);

    OUTPUT(fileOutput, "================ C++ New/Delete ================");

    PrintResults(fileOutput, result.averageDealloc, result.minimumDeallocTime, result.maximumDeallocTime, true);
    PrintResults(fileOutput, result.averageDeallocCpp, result.minimumDeallocTimeCpp, result.maximumDeallocTimeCpp, true);

    OUTPUT(fileOutput, "================================================================================================");

    fileOutput.close();
}

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

int main(int, char**)
{
    ignite::mem::MemoryManager::Init(sizeBytes);

    ignite::bench::Console::Init();

    RunTest<uint32_t>();
    RunTest<SmallerComponent>();
    RunTest<Component>();
    RunTest<LargerComponent>();

    ignite::bench::Console::Instance()->Run();

    ignite::mem::MemoryManager::Destroy();
}