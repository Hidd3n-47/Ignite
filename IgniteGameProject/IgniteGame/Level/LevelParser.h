#pragma once

#include <unordered_map>

#include <IgniteMem/Core/WeakRef.h>

#include <IgniteEngine/Core/Rendering/Texture.h>

#include "LevelState.h"

namespace ignite
{

class Scene;
class GameObject;

struct CollisionInfoRect
{
    float x, y, w, h;
};

class LevelParser
{
public:
    static void Init();
    static void LoadLevel(mem::WeakRef<Scene> scene, mem::WeakRef<GameObject> player, const LevelState state);
    static void Destroy();

    inline static constexpr int START_LINE_TEXTURE_ID{ 16 };
private:
    static Texture mLevelSpritesheet;
    static std::unordered_map<uint32_t, CollisionInfoRect> mSpritesheetIdToCollisionInfo;
};

} // Namespace ignite.