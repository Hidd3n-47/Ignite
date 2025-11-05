#pragma once

#include <filesystem>

#include <IgniteMem/Core/WeakRef.h>

namespace ignite
{

class Scene;

class LevelParser
{
public:
    static void LoadLevel(mem::WeakRef<Scene> scene, const std::filesystem::path& levelPath);
};

} // Namespace ignite.