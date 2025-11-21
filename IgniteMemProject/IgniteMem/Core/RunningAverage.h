#pragma once

#include <cstdint>

namespace ignite::mem
{

struct RunningAverage
{
    double average = 0;
    uint64_t count = 1;

    void UpdateAverage(double value)
    {
        average *= count++;
        average += value;
        average /= count;
    }
};

} // Namespace ignite::mem.
