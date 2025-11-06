#include "GameApplicationState.h"

#include <IgniteEngine/Core/Engine.h>

#include <IgniteEngine/Core/Input/Keycode.h>
#include <IgniteEngine/Core/Input/InputManager.h>

#include <IgniteEngine/EC/Components/Transform.h>
#include <IgniteEngine/EC/Components/SpriteRenderer.h>
#include <IgniteEngine/EC/Components/RaceStartCountdown.h>
#include <IgniteEngine/EC/Components/BoxCollider.h>

#include "Src/Defines.h"
#include "Level/LevelParser.h"

namespace ignite
{

GameApplicationState::GameApplicationState(const mem::WeakRef<GameApplicationStateInitInfo> info)
{
    mCurrentLevel = info->level;
}

void GameApplicationState::InitScene()
{
    mInputManager = Engine::Instance()->GetInputManager();

    mPlayer = CreateGameObject();

    LevelParser::LoadLevel(mem::WeakRef{ this }.Cast<Scene>(), mPlayer, mCurrentLevel);

    mPlayer->GetComponent<Transform>()->scale = Vec2{ 2.0f };
    mPlayer->AddComponent<SpriteRenderer>("E:/Programming/Ignite/Assets/car_24px_8way_blue_1.png", 2);
    mPlayer->AddComponent<BoxCollider>(Vec2{0.2f, 0.1f }, true)->SetOffset(Vec2{0, -0.05f});

    mRaceCountdown = CreateGameObject()->AddComponent<RaceStartCountdown>(3.9f, [&] { ChangeGameState(GameState::RACING); });

    ChangeGameState(GameState::RACE_COUNTDOWN);
}

void GameApplicationState::SceneUpdate() const
{
    if (!input)
    {
        return;
    }

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

void GameApplicationState::ChangeGameState(const GameState state)
{
    switch (state)
    {
    case GameState::RACE_COUNTDOWN:
        GAME_LOG("Changed to game state: RACE_COUNTDOWN");
        mRaceCountdown->StartTimer();
        break;
    case GameState::RACING:
        GAME_LOG("Changed to game state: RACING");
        input = true;
        break;
    default:
        GAME_ERROR("Trying to change game state to unprocessed state.");
        GAME_BREAK();
        break;
    }
}
} // Namespace ignite.