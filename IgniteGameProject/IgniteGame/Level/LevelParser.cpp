#include "LevelParser.h"

#include <fstream>

#include <IgniteEngine/EC/Scene.h>
#include <IgniteEngine/EC/Components/Transform.h>
#include <IgniteEngine/EC/Components/SpriteRenderer.h>

#include <IgniteEngine/Core/Engine.h>
#include <IgniteEngine/Core/Rendering/TextureManager.h>

#include <IgniteUtils/Xml/XmlSerializer.h>

#include "Src/Defines.h"

namespace ignite
{

Texture LevelParser::mLevelSpritesheet{};
std::unordered_map<uint32_t, CollisionInfoRect> LevelParser::mSpritesheetIdToCollisionInfo{};

void LevelParser::Init()
{
    Engine::Instance()->GetTextureManager()->Load(mLevelSpritesheet, "E:/Programming/Ignite/Assets/EnvironmentSpritesheet.png", 7, 7);

    const XmlDocument xml = XmlSerializer::Deserialize("E:/Programming/Ignite/Assets/EnvironmentSpritesheet.tsx");

    for (const XmlElement& child : xml.GetRootElement()->GetChildElements())
    {
        if (child.GetTagName() == "tile")
        {
            const uint32_t id = std::stoul(child.GetAttributeValue("id"));

            const XmlElement c = child.GetChildElements()[0].GetChildElements()[0];

            const float x = std::stof(c.GetAttributeValue("x"));
            const float y = std::stof(c.GetAttributeValue("y"));
            const float w = std::stof(c.GetAttributeValue("width"));
            const float h = std::stof(c.GetAttributeValue("height"));

            mSpritesheetIdToCollisionInfo[id] = { .x = x, .y = y, .w = w, .h = h };
        }
    }
}

void LevelParser::LoadLevel(mem::WeakRef<Scene> scene, mem::WeakRef<GameObject> player, const LevelState state)
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

                const uint32_t    delimiterOffset = delimiterPos == 0 ? 0 : 1;
                const std::string tileString      = nextPos == std::string::npos ? l.substr(delimiterPos + 1) : l.substr(delimiterPos + delimiterOffset, nextPos - delimiterPos - delimiterOffset);
                const int tileId = std::stoi(tileString);

                delimiterPos = nextPos;

                if (tileId == -1)
                {
                    continue;
                }

                float spritesheetX = static_cast<float>(tileId % 7);
                float spritesheetY = static_cast<float>(tileId / 7);

                const Vec2 position = Vec2{ x - 7.5f, -y + 4.5f };

                mem::WeakRef<GameObject> gameObject = scene->CreateGameObject();
                gameObject->AddComponent<SpriteRenderer>(mLevelSpritesheet, spritesheetX, spritesheetY, layer);
                mem::WeakRef<Transform> transform = gameObject->GetComponent<Transform>();
                transform->scale = Vec2{ scalingFactor, scalingFactor };
                transform->translation = position;

                if (tileId == START_LINE_TEXTURE_ID)
                {
                    player->GetComponent<Transform>()->translation = position;
                }
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

    mSpritesheetIdToCollisionInfo.clear();
}

} // Namespace ignite.