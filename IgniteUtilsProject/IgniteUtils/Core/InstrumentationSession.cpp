#include "InstrumentationSession.h"

namespace ignite::utils
{

void InstrumentationSession::StartSession()
{
    mFileOutput = std::ofstream("Results.json");

    mFileOutput << R"({"otherData": {},"traceEvents":[)";

    mProfileCount = 0;
}

void InstrumentationSession::WriteTime(const char* name, const uint64_t startTime, const uint64_t duration)
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

    mFileOutput.flush();
}

void InstrumentationSession::EndSession()
{
    mFileOutput << R"(]})";
    mFileOutput.close();

    delete mInstance;
    mInstance = nullptr;
}

} // Namespace ignite::utils.