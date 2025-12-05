#include <unordered_map>

#ifndef DEV_CONFIGURATION

#include "Level/LevelParser.h"
#include "Level/CollisionInfoRect.h"

void ignite::LevelParser::CreateCollisionMap()
{
    mSpritesheetIdToCollisionInfo =
    {
        { 16, ignite::CollisionInfoRect{ .x = 21.5265f, .y = 3.92184f, .w = 20.655f, .h = 56.1258f } },
        { 36, ignite::CollisionInfoRect{ .x = 9.93532f, .y = 11.6784f, .w = 44.0117f, .h = 38.6954f } },
        { 37, ignite::CollisionInfoRect{ .x = 10.0225f, .y = 3.66038f, .w = 43.7503f, .h = 46.5391f } },
        { 38, ignite::CollisionInfoRect{ .x = 10.0225f, .y = 3.74753f, .w = 43.6631f, .h = 39.3055f } },
        { 39, ignite::CollisionInfoRect{ .x = 20.0449f, .y = 14.0315f, .w = 23.7925f, .h = 23.531f } },
        { 40, ignite::CollisionInfoRect{ .x = 9.93532f, .y = 12.1141f, .w = 44.0117f, .h = 29.7188f } },
        { 41, ignite::CollisionInfoRect{ .x = 10.0225f, .y = 4.27044f, .w = 43.8374f, .h = 46.3648f } },
        { 42, ignite::CollisionInfoRect{ .x = 9.93532f, .y = 11.4169f, .w = 44.0989f, .h = 30.8518f } },
        { 43, ignite::CollisionInfoRect{ .x = 21.0908f, .y = 21.8751f, .w = 21.5265f, .h = 20.7422f } },
        { 44, ignite::CollisionInfoRect{ .x = 16.036f, .y = 25.0997f, .w = 23.8796f, .h = 14.6415f } },
        { 45, ignite::CollisionInfoRect{ .x = 5.92633f, .y = 11.7655f, .w = 51.9425f, .h = 38.5211f } },
        { 46, ignite::CollisionInfoRect{ .x = 5.92633f, .y = 10.894f, .w = 52.1168f, .h = 39.3927f } },
    };
}

#endif // !DEV_CONFIGURATION.
