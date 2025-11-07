#pragma once

#include <IgniteEngine/EC/Scene.h>

#include "ApplicationState.h"
#include "Level/LevelState.h"

namespace ignite
{

class RaceTimer;
class InputManager;
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

    void ChangeGameState(const GameState state);
private:
    LevelState mCurrentLevel;

    mem::WeakRef<GameObject>         mPlayer;
    mem::WeakRef<RaceStartCountdown> mRaceCountdown;
    mem::WeakRef<RaceTimer>          mRaceTimer;
    mem::WeakRef<InputManager>       mInputManager;
};

} // Namespace ignite.