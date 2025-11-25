#include "InstrumentationTimer.h"

#include "InstrumentationSession.h"


namespace ignite::utils
{

InstrumentationTimer::InstrumentationTimer(const char* name)
    : mName(name)
    , mStart(std::chrono::high_resolution_clock::now())
{
    // Empty.
}

InstrumentationTimer::~InstrumentationTimer()
{
    const uint64_t end   = StopTimer();

    const uint64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(mStart).time_since_epoch().count();

    InstrumentationSession::Instance()->WriteTime(mName, start, end);
}

uint64_t InstrumentationTimer::StopTimer() const
{
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - mStart);

    return duration.count();
}

} // Namespace ignite::bench.