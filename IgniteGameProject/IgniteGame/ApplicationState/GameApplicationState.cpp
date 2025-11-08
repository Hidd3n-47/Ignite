#include "GameApplicationState.h"

#include <IgniteEngine/Core/Engine.h>

#include <IgniteEngine/EC/Components/UiText.h>
#include <IgniteEngine/EC/Components/Transform.h>
#include <IgniteEngine/EC/Components/RaceTimer.h>
#include <IgniteEngine/EC/Components/CarMovement.h>
#include <IgniteEngine/EC/Components/BoxCollider.h>
#include <IgniteEngine/EC/Components/ParticleSystem.h>
#include <IgniteEngine/EC/Components/SpriteRenderer.h>
#include <IgniteEngine/EC/Components/RaceStartCountdown.h>
#include <IgniteEngine/EC/Components/RaceManagerComponent.h>

#include "Src/Defines.h"
#include "Core/GameManager.h"
#include "Level/LevelParser.h"
#include "RewardsApplicationState.h"

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

    GameManager::Instance()->GetLevelParser()->LoadLevel(mem::WeakRef{ this }.Cast<Scene>(), mPlayer, mCurrentLevel);

    mem::WeakRef<Transform> transform = mPlayer->GetComponent<Transform>();
    transform->scale = Vec2{ 2.0f };
    mPlayer->AddComponent<CarMovement>();
    mPlayer->AddComponent<SpriteRenderer>("Assets/CarBlue.png", 2);
    mPlayer->AddComponent<BoxCollider>(Vec2{0.2f, 0.1f }, true)->SetOffset(Vec2{0.0f, -0.05f});
    mPlayer->AddComponent<RaceManagerComponent>([&] { ChangeGameState(GameState::RACE_COMPLETED); });

    const ParticleEffectDetails smokeParticleDetails
    {
        .textureFilepath    = "Assets/Smoke.png",
        .textureLayer       = 2,
        .numberOfParticles  = 10,
        .position           = mem::WeakRef{ &transform->translation },
        .minPositionOffset  = Vec2{-0.1f, 0.0f },
        .maxPositionOffset  = Vec2{-0.1f, 0.0f },
        .minLifetime        = 0.4f,
        .maxLifetime        = 0.4f,
        .minScale           = 0.1f,
        .maxScale           = 0.3f,
        .particleSpawnInterval = 0.08f,
    };
    mPlayer->AddComponent<ParticleSystem>(smokeParticleDetails);

    mem::WeakRef<GameObject>   raceCountdownObject = CreateGameObject();
    const mem::WeakRef<UiText> raceCountdownText   = raceCountdownObject->AddComponent<UiText>("3", 600.0f);
    mRaceCountdown = raceCountdownObject->AddComponent<RaceStartCountdown>(raceCountdownText, 3.9f, [&] { ChangeGameState(GameState::RACING); });

    mem::WeakRef<GameObject> raceTimerObject = CreateGameObject();
    raceTimerObject->GetComponent<Transform>()->translation = Vec2{ -7.5f, 4.2f };

    mRaceTimer = raceTimerObject->AddComponent<RaceTimer>(50.0f);

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
    case GameState::RACE_COMPLETED:
    {
        GAME_LOG("Changed to game state: RACE_COMPLETED");
        const mem::WeakRef<GameManager> gameManager = GameManager::Instance();
        RewardsApplicationStateInitInfo* initInfo = new RewardsApplicationStateInitInfo(gameManager->GetTrophyRanking(mRaceTimer->Stop()));
        GameManager::Instance()->ChangeState(ApplicationStates::REWARDS, initInfo);
        break;
    }
    default:
        GAME_ERROR("Trying to change game state to unprocessed state.");
        GAME_BREAK();
        break;
    }
}
} // Namespace ignite.