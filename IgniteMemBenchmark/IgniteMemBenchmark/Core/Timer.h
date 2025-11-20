#pragma once

#include <chrono>

namespace ignite::bench
{

class Timer
{
public:
    Timer();
    ~Timer() = default;

    uint64_t StopTimer() const;
private:
    std::chrono::high_resolution_clock::time_point mStart;
};

} // Namespace ignite::bench.