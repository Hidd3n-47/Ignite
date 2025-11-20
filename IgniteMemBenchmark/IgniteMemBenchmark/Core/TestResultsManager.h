#pragma once

#include <string>
#include <vector>

namespace ignite::bench
{

struct TestResult
{
    std::string testName;

    std::vector<double> memoryAllocTimes;
    std::vector<double> memoryDeallocTimes;

    std::vector<double> memoryAllocTimesCpp;
    std::vector<double> memoryDeallocTimesCpp;

    double averageAlloc;
    double maximumAllocTime;
    double minimumAllocTime;

    double averageAllocCpp;
    double maximumAllocTimeCpp;
    double minimumAllocTimeCpp;

    double averageDealloc;
    double maximumDeallocTime;
    double minimumDeallocTime;

    double averageDeallocCpp;
    double maximumDeallocTimeCpp;
    double minimumDeallocTimeCpp;
};

class TestResultsManager
{
public:
    [[nodiscard]] inline static TestResultsManager* Instance() { return !mInstance ? mInstance = new TestResultsManager() : mInstance; }

    void AddTestResult(TestResult& result);
    [[nodiscard]] inline const std::vector<TestResult>& GetTestResults() const { return mResults; }
private:
    TestResultsManager()  = default;
    ~TestResultsManager() = default;

    inline static TestResultsManager* mInstance = nullptr;

    std::vector<TestResult> mResults;
};

} // Namespace ignite::mem.