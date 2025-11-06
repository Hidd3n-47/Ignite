#pragma once

#include <cstdint>

namespace ignite
{

struct Texture
{
    uint16_t id = INVALID_TEXTURE_ID;

    float width  = 1;
    float height = 1;

    float spritesheetMaxX = 1;
    float spritesheetMaxY = 1;

    inline static constexpr uint16_t INVALID_TEXTURE_ID{ 0 };
};

} // Namespace ignite.