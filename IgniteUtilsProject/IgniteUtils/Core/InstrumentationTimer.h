#pragma once

#include <chrono>

namespace ignite::utils
{

class InstrumentationTimer
{
public:
    InstrumentationTimer(const char* name);
    ~InstrumentationTimer();

    uint64_t StopTimer() const;
private:
    const char* mName;
    std::chrono::high_resolution_clock::time_point mStart;
};

} // Namespace ignite::bench.