#pragma once

#include <chrono>

namespace ignite::bench
{

class Timer
{
typedef std::chrono::duration<unsigned long long, std::nano> nanoseconds;
public:
    Timer();
    ~Timer() = default;

    uint64_t StopTimer() const;
private:
    nanoseconds mStart;
};

} // Namespace ignite::bench.