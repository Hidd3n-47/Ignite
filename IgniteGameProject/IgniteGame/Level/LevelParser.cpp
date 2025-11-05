#include "LevelParser.h"

#include <fstream>

#include <IgniteEngine/EC/Scene.h>
#include <IgniteEngine/EC/Components/Transform.h>
#include <IgniteEngine/EC/Components/SpriteRenderer.h>

#include "IgniteEngine/Core/Engine.h"
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

    // todo
    // What would be really ideal is if the parsed level map has characters that are the
    // spritesheet index, that way we only load one texture, and we render the spritesheet instead.

    // Parse
    constexpr float tileSize = 64.0f;
    const float a = Engine::Instance()->GetCamera().SizeInScreenSpace(Vec2{ 1.0f, 0.0f }).x;

    const float scalingFactor = a / tileSize;
    for (int y{ 0 }; y < 10; ++y)
    {
        for (int x{ 0 }; x <17; ++x)
        {
            mem::WeakRef<GameObject> gameObject = scene->CreateGameObject();

            switch (lines[y][x])
            {
            case '*':
                // Nothingness.
                break;
            case 'F':
                gameObject->AddComponent<SpriteRenderer>("E:/Programming/Ignite/Assets/png/road/road_64px_a_11.png");
                break;
            case '=':
                gameObject->AddComponent<SpriteRenderer>("E:/Programming/Ignite/Assets/png/road/road_64px_a_2.png");
                break;
            case 'L':
                gameObject->AddComponent<SpriteRenderer>("E:/Programming/Ignite/Assets/png/road/road_64px_a_11.png");
                gameObject->GetComponent<Transform>()->rotation = 270.0f;
                break;
            case '7':
                gameObject->AddComponent<SpriteRenderer>("E:/Programming/Ignite/Assets/png/road/road_64px_a_11.png");
                gameObject->GetComponent<Transform>()->rotation = 90;
                break;
            case 'U':
                gameObject->AddComponent<SpriteRenderer>("E:/Programming/Ignite/Assets/png/road/road_64px_a_11.png");
                gameObject->GetComponent<Transform>()->rotation = 180.0f;
                break;
            case '|':
                gameObject->AddComponent<SpriteRenderer>("E:/Programming/Ignite/Assets/png/road/road_64px_a_3.png");
                break;
            }

            //gameObject->AddComponent<SpriteRenderer>("E:/Programming/Ignite/Assets/png/road/road_64px_a_2.png");
            gameObject->GetComponent<Transform>()->scale = Vec2{ scalingFactor, scalingFactor };
            gameObject->GetComponent<Transform>()->translation = Vec2{ static_cast<float>(x) - 8.0f, -static_cast<float>(y) + 4.5f };
        }
    }
}


} // Namespace ignite.