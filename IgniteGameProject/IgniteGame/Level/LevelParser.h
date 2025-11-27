#pragma once

#include <unordered_map>

#include <IgniteMem/Core/WeakHandle.h>

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
    void Init();
    void LoadLevel(mem::WeakHandle<Scene> scene, mem::WeakHandle<GameObject> player, const LevelState state);
    void Destroy();

    inline static constexpr int START_LINE_TEXTURE_ID{ 16 };
private:
    Texture mLevelSpritesheet;
    std::unordered_map<uint32_t, CollisionInfoRect> mSpritesheetIdToCollisionInfo;
};

} // Namespace ignite.