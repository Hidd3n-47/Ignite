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

#ifdef DEV_CONFIGURATION
    bool debugSquare;
    Vec2 debugSquareOffset;
    Vec2 debugSquareHalfExtents;
#endif // DEV_CONFIGURATION.
};

} // Namespace ignite.