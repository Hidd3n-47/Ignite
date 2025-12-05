#pragma once
    
#include <IgniteEngine/EC/Scene.h>
#include <IgniteEngine/EC/Components/Transform.h>
#include <IgniteEngine/EC/Components/BoxCollider.h>
#include <IgniteEngine/EC/Components/SpriteRenderer.h>
#include <IgniteEngine/EC/Components/RaceManagerComponent.h>

#include <IgniteEngine/Core/Engine.h>

#include "Core/GameManager.h"
#include "Level/LevelParser.h"
#include "Level/CollisionInfoRect.h"

namespace ignite
{

static void BuildTile(const uint32_t tileId, const uint32_t layer, const uint32_t x, const uint32_t y, 
                      const OrthoCamera& camera, const float tileSize, const float scalingFactor, mem::WeakHandle<Scene> scene, mem::WeakHandle<GameObject> player, const Texture& levelSpritesheet, std::unordered_map<uint32_t, CollisionInfoRect>& collisionMap)
{
    const float spritesheetX = static_cast<float>(tileId % 7);
    const float spritesheetY = static_cast<float>(tileId / 7);
    const Vec2 position{ static_cast<float>(x) - 7.5f, -static_cast<float>(y) + 4.5f };

    mem::WeakHandle<GameObject> gameObject = scene->CreateGameObject();

    gameObject->AddComponent<SpriteRenderer>(levelSpritesheet, spritesheetX, spritesheetY, layer);

    mem::WeakHandle<Transform> transform = gameObject->GetComponent<Transform>();
    transform->scale       = Vec2{scalingFactor, scalingFactor};
    transform->translation = position;

    if (tileId == LevelParser::START_LINE_TEXTURE_ID)
    {
        player->GetComponent<Transform>()->translation = position - Vec2{ 0.4f, 0.0f };
    }

    if (collisionMap.contains(tileId))
    {
        const CollisionInfoRect rect = collisionMap[tileId];

        const Vec2 centre      = Vec2{rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f};
        const Vec2 offset      = centre - Vec2{tileSize * 0.5f};
        const Vec2 centreWorld = camera.ScreenSizeToWorldSize(offset);
        const Vec2 sizeWorld   = camera.ScreenSizeToWorldSize(Vec2{rect.w, rect.h});

        mem::WeakHandle<BoxCollider> box = gameObject->AddComponent<BoxCollider>(sizeWorld, false, tileId == LevelParser::START_LINE_TEXTURE_ID);
        box->SetOffset(Vec2{centreWorld.x, -centreWorld.y});
        box->SetOnTriggeredCallback([](mem::WeakHandle<GameObject> other) -> void {
            other->GetComponent<RaceManagerComponent>()->PassedStartFinishLine();
        });
    }
}

} // Namespace ignite.
