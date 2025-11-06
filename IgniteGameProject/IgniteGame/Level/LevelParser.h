#pragma once

#include <IgniteMem/Core/WeakRef.h>

#include <IgniteEngine/Core/Rendering/Texture.h>

#include "LevelState.h"

namespace ignite
{

class Scene;

class LevelParser
{
public:
    static void Init();
    static void LoadLevel(mem::WeakRef<Scene> scene, const LevelState state);
    static void Destroy();

private:
    static Texture mLevelSpritesheet;
};

} // Namespace ignite.