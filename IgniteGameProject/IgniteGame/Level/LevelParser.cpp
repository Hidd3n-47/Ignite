#include "LevelParser.h"

#include <fstream>

#include <IgniteEngine/EC/Scene.h>
#include <IgniteEngine/EC/Components/Transform.h>
#include <IgniteEngine/EC/Components/SpriteRenderer.h>

#include "Src/Defines.h"
#include "IgniteEngine/Core/Engine.h"
#include "IgniteEngine/Core/Rendering/TextureManager.h"

namespace ignite
{

Texture LevelParser::mLevelSpritesheet{};

void LevelParser::Init()
{
    Engine::Instance()->GetTextureManager()->Load(mLevelSpritesheet, "E:/Programming/Ignite/Assets/EnvironmentSpritesheet.png", 7, 7);
}

void LevelParser::LoadLevel(mem::WeakRef<Scene> scene, const LevelState state)
{
    const std::filesystem::path levelPath{ "E:/Programming/Ignite/Assets/Levels/Level" + std::to_string(static_cast<uint8_t>(state)) };

    std::vector<std::string> lines;
    std::string line;

    std::string levelFiles[2] = { "Track.csv", "BG.csv" };

    uint32_t layer = 0;
    for (const std::string& levelFile : levelFiles)
    {
        std::ifstream fileInput;
        fileInput.open(levelPath / levelFile, std::ios::in);

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

        // Parse
        const float tileSize = mLevelSpritesheet.width;
        const float tileSizeScreenspace = Engine::Instance()->GetCamera().SizeInScreenSpace(Vec2{ 1.0f, 0.0f }).x;
        const float scalingFactor = tileSizeScreenspace / tileSize;

        float y = 0;
        for (const std::string& l : lines)
        {
            float x = -1;
            std::size_t delimiterPos = 0;
            while (delimiterPos != std::string::npos)
            {
                x++;

                const std::size_t nextPos = l.find(',', delimiterPos + 1);

                const uint32_t delimiterOffset = delimiterPos == 0 ? 0 : 1;
                const std::string tileString = nextPos == std::string::npos ? l.substr(delimiterPos + 1) : l.substr(delimiterPos + delimiterOffset, nextPos - delimiterPos - delimiterOffset);
                const int tileId = std::stod(tileString);

                delimiterPos = nextPos;

                if (tileId == -1)
                {
                    continue;
                }

                float spritesheetX = static_cast<float>(tileId % 7);
                float spritesheetY = static_cast<float>(tileId / 7);

                mem::WeakRef<GameObject> gameObject = scene->CreateGameObject();
                gameObject->AddComponent<SpriteRenderer>(mLevelSpritesheet, spritesheetX, spritesheetY, layer);
                gameObject->GetComponent<Transform>()->scale = Vec2{ scalingFactor, scalingFactor };
                gameObject->GetComponent<Transform>()->translation = Vec2{ x - 7.5f, -y + 4.5f };

            }
            ++y;
        }

        lines.clear();
        ++layer;
    }
}

void LevelParser::Destroy()
{
    Engine::Instance()->GetTextureManager()->RemoveTexture(mLevelSpritesheet.id);
}

} // Namespace ignite.