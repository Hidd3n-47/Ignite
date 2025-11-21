#include "TestResultsManager.h"

namespace ignite::bench
{

void TestResultsManager::AddTestResult(TestResult& result)
{
    mResults.emplace_back(result);
    {
        double total = 0.0;
        double max   = 0.0;
        double min   = std::numeric_limits<double>::max();
        for (const double t : mResults.back().memoryAllocTimes)
        {
            total += t;
            max = std::max(t, max);
            min = std::min(t, min);
        }
        mResults.back().averageAlloc     = total / static_cast<double>(mResults.back().memoryAllocTimes.size());
        mResults.back().minimumAllocTime = min;
        mResults.back().maximumAllocTime = max;
    }

    {
        double total = 0.0;
        double max   = 0.0;
        double min   = std::numeric_limits<double>::max();
        for (const double t : mResults.back().memoryAllocTimesCpp)
        {
            total += t;
            max = std::max(t, max);
            min = std::min(t, min);
        }
        mResults.back().averageAllocCpp = total / static_cast<double>(mResults.back().memoryAllocTimesCpp.size());
        mResults.back().minimumAllocTimeCpp = min;
        mResults.back().maximumAllocTimeCpp = max;
    }

    {
        double total = 0.0;
        double max   = 0.0;
        double min   = std::numeric_limits<double>::max();
        for (const double t : mResults.back().memoryDeallocTimes)
        {
            total += t;
            max = std::max(t, max);
            min = std::min(t, min);
        }
        mResults.back().averageDealloc     = total / static_cast<double>(mResults.back().memoryDeallocTimes.size());
        mResults.back().minimumDeallocTime = min;
        mResults.back().maximumDeallocTime = max;
    }

    {
        double total = 0.0;
        double max   = 0.0;
        double min   = std::numeric_limits<double>::max();
        for (const double t : mResults.back().memoryDeallocTimesCpp)
        {
            total += t;
            max = std::max(t, max);
            min = std::min(t, min);
        }
        mResults.back().averageDeallocCpp     = total / static_cast<double>(mResults.back().memoryDeallocTimesCpp.size());
        mResults.back().minimumDeallocTimeCpp = min;
        mResults.back().minimumDeallocTimeCpp = max;
    }

    result.averageAlloc             = mResults.back().averageAlloc;
    result.maximumAllocTime         = mResults.back().maximumAllocTime;
    result.minimumAllocTime         = mResults.back().minimumAllocTime;
    result.averageAllocCpp          = mResults.back().averageAllocCpp;
    result.maximumAllocTimeCpp      = mResults.back().maximumAllocTimeCpp;
    result.minimumAllocTimeCpp      = mResults.back().minimumAllocTimeCpp;
    result.averageDealloc           = mResults.back().averageDealloc;
    result.maximumDeallocTime       = mResults.back().maximumDeallocTime;
    result.minimumDeallocTime       = mResults.back().minimumDeallocTime;
    result.averageDeallocCpp        = mResults.back().averageDeallocCpp;
    result.maximumDeallocTimeCpp    = mResults.back().maximumDeallocTimeCpp;
    result.minimumDeallocTimeCpp    = mResults.back().minimumDeallocTimeCpp;
}

} // Namespace ignite::bench.