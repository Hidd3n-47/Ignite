#pragma once

namespace ignite
{

struct OrthoCameraValues
{
    float left, right, top, bottom;
    Vec2 screenDimensions;
};

/**
 * @class OrthoCamera: A class representing an orthographic camera.
 */
class OrthoCamera
{
public:
    OrthoCamera() = default;
    OrthoCamera(const OrthoCameraValues& values);

    /**
     * @brief Convert screen-space coordinates into world space coordinates.
     * @param screenSpace The coordinates in screen-space (pixels).
     * @return The world-space that represents the same screen-space.
     */
    [[nodiscard]] Vec2 ScreenToWorld(const Vec2 screenSpace) const;
    /**
     * @brief Convert a size (in pixels) to a size in world-space.
     * @param screenSpace The screen-space size (measured in pixels).
     * @return The world-space size that represents the same size in screen-space (pixels).
     */
    [[nodiscard]] Vec2 ScreenSizeToWorldSize(const Vec2 screenSpace) const;
    /**
     * @brief Convert a world-space position to screen-space.
     * @param position The world-space position that needs to be converted to screen-space.
     * @return The screen-space coordinates that represents the world-space that was passed in.
     */
    [[nodiscard]] Vec2 PositionToScreenSpace(const Vec2 position) const;
    /**
     * @brief Convert a world-space size to screen-space size (in pixels).
     * @param size The world-space size that needs to be converted to screen-space.
     * @return The screen-space size (pixels) that represents the world-space size.
     */
    [[nodiscard]] inline Vec2 SizeInScreenSpace(const Vec2 size) const { return PositionToScreenSpace(size * Vec2{ 1.0f, -1.0f }) - mScreenCoordsOfCentre; }

    /**
     * @brief Get the dimensions of the screen.
     * @return The screen dimensions.
     */
    [[nodiscard]] inline Vec2 GetScreenDimensions() const { return mScreenDimensions; }
    /**
     * @brief Set the dimensions of the screen.
     * @param screenDimensions The new dimensions of the screen.
     */
    inline void SetScreenDimensions(const Vec2 screenDimensions) { mScreenDimensions = screenDimensions; mScreenCoordsOfCentre = mScreenDimensions * 0.5f; }
private:
    float mLeft, mRight;
    float mTop, mBottom;

    Vec2 mScreenDimensions;
    Vec2 mScreenCoordsOfCentre;
};

} // Namespace ignite.