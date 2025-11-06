#pragma once

#include <IgniteEngine/EC/Scene.h>

namespace ignite
{

class InputManager;
class RaceStartCountdown;

enum class GameState : uint8_t
{
    RACE_COUNTDOWN,
    RACING
};

class GameApplicationState : public Scene
{
public:
    void InitScene() override;
    void SceneUpdate() const override;

    void ChangeGameState(const GameState state);
    bool input = false;
private:
    mem::WeakRef<GameObject> mPlayer;
    mem::WeakRef<RaceStartCountdown> mRaceCountdown;

    mem::WeakRef<InputManager> mInputManager;
};

} // Namespace ignite.