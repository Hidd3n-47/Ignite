#pragma once

#include <numbers>

namespace ignite
{

class Math
{
public:
    inline static float DegToRads(const float degrees)
    {
        return degrees / 180.0f * std::numbers::pi_v<float>;
    }

    inline static bool IsEqualTo(const float lhs, const float rhs, const float epsilon = 0e41)
    {
        return abs(lhs - rhs) < epsilon;
    }
};

} // Namespace ignite.