#include "LevelParser.h"

#include <IgniteEngine/EC/Scene.h>
#include <IgniteEngine/EC/Components/Transform.h>
#include <IgniteEngine/EC/Components/BoxCollider.h>
#include <IgniteEngine/EC/Components/SpriteRenderer.h>
#include <IgniteEngine/EC/Components/RaceManagerComponent.h>

#include <IgniteEngine/Core/Engine.h>
#include <IgniteEngine/Core/Rendering/TextureManager.h>

#ifdef DEV_CONFIGURATION
#include <fstream>
#include <IgniteUtils/Xml/XmlSerializer.h>
#else // Else DEV_CONFIGURATION.
//#include "Build/GeneratedCollisionInfo.cpp"
#include "Build/GeneratedLevels.h"
#endif // !DEV_CONFIGURATION.

#include "Core/GameManager.h"
#include "Src/Defines.h"

namespace ignite
{

void LevelParser::Init()
{
    PROFILE_FUNC();

    Engine::Instance()->GetTextureManager()->Load(mLevelSpritesheet, "Assets/EnvironmentSpritesheet.png", 7, 7);

#ifdef DEV_CONFIGURATION
    const XmlDocument xml = XmlSerializer::Deserialize("Assets/EnvironmentSpritesheet.tsx");

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
#else // !DEV_CONFIGURATION.
    CreateCollisionMap();
#endif // !DEV_CONFIGURATION.
}

void LevelParser::LoadLevel(mem::WeakHandle<Scene> scene, mem::WeakHandle<GameObject> player, const LevelState state)
{
    PROFILE_FUNC();

#ifdef DEV_CONFIGURATION
    const std::filesystem::path levelPath{ "Assets/Levels/Level" + std::to_string(static_cast<uint8_t>(state)) };

    std::vector<std::string> lines;
    std::string line;

    const std::string levelFiles[2] = { "Track.csv", "BG.csv" };

    const OrthoCamera& camera = Engine::Instance()->GetCamera();

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

                mem::WeakHandle<GameObject> gameObject = scene->CreateGameObject();
                gameObject->AddComponent<SpriteRenderer>(mLevelSpritesheet, spritesheetX, spritesheetY, layer);
                mem::WeakHandle<Transform> transform = gameObject->GetComponent<Transform>();
                transform->scale = Vec2{ scalingFactor, scalingFactor };
                transform->translation = position;

                const bool startLine = tileId == START_LINE_TEXTURE_ID;

                if (startLine)
                {
                    player->GetComponent<Transform>()->translation = position - Vec2{ 0.4f, 0.0f };
                }

                if (mSpritesheetIdToCollisionInfo.contains(tileId))
                {
                    CollisionInfoRect rect = mSpritesheetIdToCollisionInfo[tileId];

                    const Vec2 centre = Vec2{ rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f };
                    const Vec2 offset = centre - Vec2{ tileSize * 0.5f };
                    const Vec2 centreWorld = camera.ScreenSizeToWorldSize(offset);

                    const Vec2 sizeWorld = camera.ScreenSizeToWorldSize(Vec2{ rect.w, rect.h });

                    mem::WeakHandle<BoxCollider> box = gameObject->AddComponent<BoxCollider>(sizeWorld, false, startLine);
                    box->SetOffset(Vec2{ centreWorld.x, -centreWorld.y });
                    box->SetOnTriggeredCallback([](mem::WeakHandle<GameObject> other) { other->GetComponent<RaceManagerComponent>()->PassedStartFinishLine(); });
                }
            }
            ++y;
        }

        lines.clear();
        ++layer;
    }

    {

        std::ifstream fileInput;
        fileInput.open(levelPath / "Ranks.lvl", std::ios::in);

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

        GameManager::Instance()->SetLevelRankTimes(std::stof(lines[0]), std::stof(lines[1]), std::stof(lines[2]));
    }
#else // Else DEV_CONFIGURATION.
    switch (state)
    {
    case LevelState::ONE:
        BuildLevel1(scene, player, mLevelSpritesheet, mSpritesheetIdToCollisionInfo);
        break;
    case LevelState::TWO:
        BuildLevel2(scene, player, mLevelSpritesheet, mSpritesheetIdToCollisionInfo);
        break;
    }
#endif // !DEV_CONFIGURATION.
}

void LevelParser::Destroy()
{
    Engine::Instance()->GetTextureManager()->RemoveTexture(mLevelSpritesheet.id);

    mSpritesheetIdToCollisionInfo.clear();
}

} // Namespace ignite.