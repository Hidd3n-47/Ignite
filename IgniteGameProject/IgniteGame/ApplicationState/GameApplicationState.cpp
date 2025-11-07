#include "GameApplicationState.h"

#include <IgniteEngine/Core/Engine.h>

#include <IgniteEngine/EC/Components/UiText.h>
#include <IgniteEngine/EC/Components/Transform.h>
#include <IgniteEngine/EC/Components/RaceTimer.h>
#include <IgniteEngine/EC/Components/CarMovement.h>
#include <IgniteEngine/EC/Components/BoxCollider.h>
#include <IgniteEngine/EC/Components/SpriteRenderer.h>
#include <IgniteEngine/EC/Components/RaceStartCountdown.h>

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
    mPlayer->AddComponent<CarMovement>();
    mPlayer->AddComponent<SpriteRenderer>("E:/Programming/Ignite/Assets/CarBlue.png", 2);
    mPlayer->AddComponent<BoxCollider>(Vec2{0.2f, 0.1f }, true)->SetOffset(Vec2{0, -0.05f});

    mem::WeakRef<GameObject>   raceCountdownObject = CreateGameObject();
    const mem::WeakRef<UiText> raceCountdownText   = raceCountdownObject->AddComponent<UiText>("3", 600.0f);
    mRaceCountdown = raceCountdownObject->AddComponent<RaceStartCountdown>(raceCountdownText, 3.9f, [&] { ChangeGameState(GameState::RACING); });

    mem::WeakRef<GameObject> raceTimerObject = CreateGameObject();
    raceTimerObject->GetComponent<Transform>()->translation = Vec2{ -7.5f, 4.2f };

    mRaceTimer = raceTimerObject->AddComponent<RaceTimer>(50.0);

    ChangeGameState(GameState::RACE_COUNTDOWN);
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
        mPlayer->GetComponent<CarMovement>()->EnableMovement();
        mRaceTimer->StartTimer();
        break;
    default:
        GAME_ERROR("Trying to change game state to unprocessed state.");
        GAME_BREAK();
        break;
    }
}
} // Namespace ignite.