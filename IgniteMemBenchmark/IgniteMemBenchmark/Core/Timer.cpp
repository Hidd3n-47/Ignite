#include "Timer.h"

namespace ignite::bench
{

Timer::Timer()
    : mStart(std::chrono::high_resolution_clock::now().time_since_epoch())
{
    // Empty.
}

uint64_t Timer::StopTimer() const
{
    const auto time = std::chrono::high_resolution_clock::now().time_since_epoch() - mStart;

    return time.count();
}

} // Namespace ignite::bench.