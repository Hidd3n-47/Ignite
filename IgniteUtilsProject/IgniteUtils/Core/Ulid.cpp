#include "Ulid.h"

#include <chrono>
#include <random>

namespace ignite
{

namespace
{
std::random_device   gDevice;
std::mt19937         gRandomEngine{ gDevice() };
std::uniform_int_distribution<uint32_t> gUniformDistribution;
} // Anonymous namespace.

Ulid::Ulid()
{
    const uint32_t time = GetTimeFromEpoch();
    const uint32_t rand = GetRandomNumber();

    mUlid = static_cast<uint64_t>(time) << 32 | rand;
}

uint32_t Ulid::GetTimeFromEpoch()
{
    using namespace std::chrono;
    constexpr sys_days epoch = 2025y / 1 / 1;

    const time_point now = floor<seconds>(system_clock::now());
    return static_cast<int>((now - epoch) / 1s);
}

uint32_t Ulid::GetRandomNumber()
{
    return gUniformDistribution(gRandomEngine);
}

} //Namespace ignite.
