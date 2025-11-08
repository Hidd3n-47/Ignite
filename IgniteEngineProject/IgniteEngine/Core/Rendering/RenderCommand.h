#pragma once

#include "IgniteEngine/Core/Rendering/Texture.h"

namespace ignite
{

class Transform;

struct RenderCommand
{
    Texture texture{ };

    float spritesheetPosX = 0;
    float spritesheetPosY = 0;
    float spritesheetMaxX = 1;
    float spritesheetMaxY = 1;

    mem::WeakRef<Transform> transform;
    Vec2 offset;

    uint8_t alpha = 255;

#ifdef DEV_CONFIGURATION
    bool debugSquare;
    Vec2 debugSquareHalfExtents;
#endif // DEV_CONFIGURATION.
};

} // Namespace ignite.