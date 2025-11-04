#include "IgnitePch.h"
#include "OrthoCamera.h"

namespace ignite
{

OrthoCamera::OrthoCamera(const OrthoCameraValues& values)
    : mLeft(values.left)
    , mRight(values.right)
    , mTop(values.top)
    , mBottom(values.bottom)
    , mScreenDimensions(values.screenDimensions)
    , mScreenCoordsOfCentre{ mScreenDimensions * 0.5f }
{
    // Empty.
}

Vec2 OrthoCamera::ScreenToWorld(const Vec2 screenSpace) const
{
    Vec2 worldSpace = screenSpace / mScreenDimensions;
    worldSpace *= 2.0f;
    worldSpace -= Vec2{ 1.0f };
    worldSpace *= Vec2{ (mRight - mLeft) * 0.5f, (mBottom - mTop) * 0.5f };

    return worldSpace;
}

Vec2 OrthoCamera::ScreenSizeToWorldSize(const Vec2 screenSpace) const
{
    Vec2 worldSpace = screenSpace / mScreenDimensions;
    worldSpace *= 2.0f;

    worldSpace *= Vec2{ mRight, mTop };

    return worldSpace;
}

Vec2 OrthoCamera::PositionToScreenSpace(const Vec2 position) const
{
    Vec2 screenspace = position / Vec2{ (mRight - mLeft) * 0.5f, (mBottom - mTop) * 0.5f };
    screenspace += Vec2{ 1.0f };
    screenspace /= 2.0f;
    screenspace *= mScreenDimensions;

    return screenspace;
}

} // Namespace ignite.