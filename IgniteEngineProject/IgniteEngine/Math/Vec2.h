#pragma once

#include <cmath>

namespace ignite
{

struct Vec2
{
public:
    Vec2() = default;
    constexpr explicit Vec2(const float scalar) : x{ scalar }, y{ scalar } { /* Empty. */}
    constexpr explicit Vec2(const float x, const float y) : x{ x }, y{ y } { /* Empty. */}

    float x = 0.0f, y = 0.0f;

    [[nodiscard]] inline static Vec2 Abs(const Vec2 vec)
    {
        return Vec2{ abs(vec.x), abs(vec.y) };
    }

    [[nodiscard]] inline float Magnitude() const
    {
        return sqrt(x * x + y * y);
    }

    [[nodiscard]] inline float MagnitudeSquared() const
    {
        return x * x + y * y;
    }

    inline void Normalize()
    {
        if (const float len = sqrt(x * x + y * y); len != 0.0f)
        {
            x /= len;
            y /= len;
        }
    }

    [[nodiscard]] constexpr inline static float Dot(const Vec2 lhs, const Vec2 rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    [[nodiscard]] constexpr inline Vec2 operator+(const Vec2 rhs) const
    {
        return Vec2{ x + rhs.x, y + rhs.y };
    }

    [[nodiscard]] constexpr inline Vec2 operator-(const Vec2 rhs) const
    {
        return Vec2{ x - rhs.x, y - rhs.y };
    }

    [[nodiscard]] constexpr inline Vec2 operator-(const float scalar) const
    {
        return Vec2{ x - scalar, y - scalar };
    }

    [[nodiscard]] constexpr inline Vec2 operator*(const Vec2 rhs) const
    {
        return Vec2{ x * rhs.x, y * rhs.y };
    }

    [[nodiscard]] constexpr inline Vec2 operator*(const float scalar) const
    {
        return Vec2{ x * scalar, y * scalar };
    }

    [[nodiscard]] constexpr inline Vec2 operator/(const float scalar) const
    {
        return Vec2{ x / scalar, y / scalar };
    }

    [[nodiscard]] constexpr inline Vec2 operator/(const Vec2 vec) const
    {
        return Vec2{ x / vec.x, y / vec.y };
    }

    inline Vec2 operator+=(const Vec2 rhs)
    {
        x += rhs.x;
        y += rhs.y;

        return *this;
    }

    inline Vec2 operator-=(const Vec2 rhs)
    {
        x -= rhs.x;
        y -= rhs.y;

        return *this;
    }

    inline Vec2 operator-=(const float scalar)
    {
        x -= scalar;
        y -= scalar;

        return *this;
    }

    inline Vec2 operator*=(const float scalar)
    {
        x *= scalar;
        y *= scalar;

        return *this;
    }

    inline Vec2 operator*=(const Vec2 vec)
    {
        x *= vec.x;
        y *= vec.y;

        return *this;
    }

    inline Vec2 operator/=(const float scalar)
    {
        x /= scalar;
        y /= scalar;

        return *this;
    }
};

} // Namespace ignite.