#pragma once

#include <IgniteEngine/EC/Scene.h>

#include "ApplicationState.h"
#include "Level/LevelState.h"

namespace ignite
{

class RaceTimer;
class CarMovement;
class InputManager;
class ParticleSystem;
class RaceStartCountdown;

enum class GameState : uint8_t
{
    RACE_COUNTDOWN,
    RACING,
    RACE_COMPLETED
};

class GameApplicationStateInitInfo : public IApplicationStateInitInfo
{
public:
    GameApplicationStateInitInfo(const LevelState level) : level(level) { }

    LevelState level;
};

class GameApplicationState : public Scene
{
public:
    GameApplicationState(const mem::WeakRef<GameApplicationStateInitInfo> info);

    void InitScene() override;
    void SceneUpdate() override;

    void ChangeGameState(const GameState state);
private:
    LevelState mCurrentLevel;

    mem::WeakRef<GameObject>         mPlayer;
    mem::WeakRef<CarMovement>        mPlayerMovement;
    mem::WeakRef<ParticleSystem>     mPlayerWheelParticlesTop;
    mem::WeakRef<ParticleSystem>     mPlayerWheelParticlesBot;
    mem::WeakRef<RaceStartCountdown> mRaceCountdown;
    mem::WeakRef<RaceTimer>          mRaceTimer;
    mem::WeakRef<InputManager>       mInputManager;
};

} // Namespace ignite.