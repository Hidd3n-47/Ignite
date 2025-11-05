#include "LevelParser.h"

#include <fstream>

#include <IgniteEngine/EC/Scene.h>
#include <IgniteEngine/EC/Components/Transform.h>
#include <IgniteEngine/EC/Components/SpriteRenderer.h>

#include "Src/Defines.h"

namespace ignite
{

void LevelParser::LoadLevel(mem::WeakRef<Scene> scene, const std::filesystem::path& levelPath)
{
    std::vector<std::string> lines;
    std::string line;

    std::ifstream fileInput;
    fileInput.open(levelPath, std::ios::in);

    if (fileInput.fail())
    {
        GAME_DEBUG(std::string path = levelPath.string());
        GAME_ERROR("Failed to parse level file: {}", path.c_str());
    }

    while (std::getline(fileInput, line, '\n'))
    {
        lines.push_back(line);
    }

    fileInput.close();

    // Ensure that the file is the correct size.
    // todo.
    // Should be 12 rows, and each row shouldn't be longer than 16 characters.

    // Parse
    for (float y{ -4.5f }; y < 4.5f; ++y)
    {
        for (float x{ -8.0f }; x < 8.0f; ++x)
        {
            mem::WeakRef<GameObject> gameObject = scene->CreateGameObject();
            gameObject->AddComponent<SpriteRenderer>("E:/Programming/Ignite/Assets/png/road/road_64px_a_2.png");
            //gameObject->GetComponent<Transform>()->translation = Vec2{ x, y };
        }
    }
}


} // Namespace ignite.