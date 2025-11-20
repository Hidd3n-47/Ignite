#include "Timer.h"

namespace ignite::bench
{

Timer::Timer()
    : mStart(std::chrono::high_resolution_clock::now())
{
    // Empty.
}

uint64_t Timer::StopTimer() const
{
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - mStart);

    return duration.count();
}

} // Namespace ignite::bench.