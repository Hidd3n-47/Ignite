#include "GameApplicationState.h"

#include <IgniteEngine/Core/Engine.h>

#include <IgniteEngine/Core/Input/Keycode.h>
#include <IgniteEngine/Core/Input/InputManager.h>

#include <IgniteEngine/EC/Components/Transform.h>
#include <IgniteEngine/EC/Components/SpriteRenderer.h>

#include "Level/LevelParser.h"

namespace ignite
{

void GameApplicationState::InitScene()
{
    mInputManager = Engine::Instance()->GetInputManager();

    mPlayer = CreateGameObject();

    LevelParser::LoadLevel(mem::WeakRef{ this }.Cast<Scene>(), "E:/Programming/Ignite/Assets/Levels/Simple.lvl");

    mPlayer->GetComponent<Transform>()->scale = Vec2{ 2.0f };
    mPlayer->AddComponent<SpriteRenderer>("E:/Programming/Ignite/Assets/car_24px_8way_blue_1.png");
}

void GameApplicationState::SceneUpdate() const
{
    Vec2 direction;
    if (mInputManager->IsKeyDown(Keycode::KEY_W))
    {
        direction += Vec2{ 0.0f, 1.0f };
    }
    if (mInputManager->IsKeyDown(Keycode::KEY_A))
    {
        direction -= Vec2{ 1.0f, 0.0f };
    }
    if (mInputManager->IsKeyDown(Keycode::KEY_S))
    {
        direction -= Vec2{ 0.0f, 1.0f };
    }
    if (mInputManager->IsKeyDown(Keycode::KEY_D))
    {
        direction += Vec2{ 1.0f, 0.0f };
    }

    direction.Normalize();

    constexpr float speed = 0.1f;

    mPlayer->GetComponent<Transform>()->translation += direction * speed;
}

} // Namespace ignite.