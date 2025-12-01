#include "InstrumentationSession.h"

#include <future>
#include <iostream>

namespace ignite::utils
{

void InstrumentationSession::Create()
{
    void* memoryAddress = malloc(sizeof(InstrumentationSession));
    mInstance = new (memoryAddress) InstrumentationSession{};
}

void InstrumentationSession::Destroy()
{
    mInstance->~InstrumentationSession();
    free(mInstance);
    mInstance = nullptr;
}

void InstrumentationSession::StartSession()
{
    mFileOutput = std::ofstream("Results.json");
    mRunning    = !mFileOutput.fail();

    mFileOutput << R"({"otherData": {},"traceEvents":[)";

    mProfileCount = 0;
    mFlushThread  = std::thread{ [&] { Run(); } };
}

void InstrumentationSession::Run()
{
    while (mRunning)
    {
        Flush();
    }
}

void InstrumentationSession::ProfileFunctionTime(const char* name, const uint64_t startTime, const uint64_t duration)
{
    std::lock_guard lock(mFlushMutex);
    mFunctionTimes.emplace_back(name, startTime, duration);
}

void InstrumentationSession::EndSession()
{
    mRunning = false;

    if (mFlushThread.joinable())
    {
        mFlushThread.join();
    }

    // Preform final flush to ensure that the times are all outputted.
    Flush(true);

    mFileOutput << R"(]})";
    mFileOutput.close();
}

void InstrumentationSession::Flush(bool forceFlush)
{
    std::vector<FunctionProfile, HeapAllocator<FunctionProfile>> times;
    {
        std::lock_guard lock(mFlushMutex);
        mFunctionTimes.swap(times);
    }

    for (const auto [name, startTime, duration] : times)
    {
        if (mProfileCount++ > 0)
        {
            mFileOutput << ",";
        }

        mFileOutput << R"({"cat":"function","dur":)";
        mFileOutput << duration;
        mFileOutput << R"(,"name":")";
        mFileOutput << name;
        mFileOutput << R"(","ph":"X","pid":0,"tid":0,"ts":)";
        mFileOutput << startTime;
        mFileOutput << R"(})";
    }

    constexpr uint32_t COUNT_BEFORE_FLUSH{ 1'000 };
    if (mProfileCount >= COUNT_BEFORE_FLUSH || forceFlush)
    {
        mFileOutput.flush();

        mProfileCount = 1;
    }
}

} // Namespace ignite::utils.