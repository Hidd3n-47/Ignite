#pragma once

#include "IgniteEngine/Math/Vec2.h"

namespace ignite
{

struct Mat2
{
    float m11, m12, m21, m22;

    inline Vec2 operator*(const Vec2& vec) const
    {
        Vec2 result;
        result.x = vec.x * m11 + vec.y * m21;
        result.y = vec.x * m12 + vec.y * m22;

        return result;
    }
};

} // Namespace ignite.