#pragma once

#include <random>

namespace ignite
{

class Random
{
public:
    template <typename T>
    static T GetRandomBetween(const T lowerBoundInclusive, const T upperBoundExclusive);
private:
    inline static std::mt19937 mRandomEngine{ std::random_device{}() };
};

template <>
inline int Random::GetRandomBetween(const int lowerBoundInclusive, const int upperBoundExclusive)
{
    return static_cast<int>(GetRandomBetween<float>(static_cast<float>(lowerBoundInclusive), static_cast<float>(upperBoundExclusive)));
}

template <typename T>
inline T Random::GetRandomBetween(const T lowerBoundInclusive, const T upperBoundExclusive)
{
    std::uniform_real_distribution<T> distribution{ lowerBoundInclusive, upperBoundExclusive };
    return distribution(mRandomEngine);
}

} // Namespace ignite.
