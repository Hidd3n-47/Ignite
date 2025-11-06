#pragma once

#include <IgniteMem/Core/WeakRef.h>

#include "ApplicationState/ApplicationState.h"

namespace ignite
{

class Scene;

class GameManager
{
public:
    [[nodiscard]] inline static mem::WeakRef<GameManager> Instance() { return mem::WeakRef{ mInstance }; }

    GameManager(const GameManager&)             = delete;
    GameManager(GameManager&&)                  = delete;
    GameManager& operator=(GameManager&&)       = delete;
    GameManager& operator=(const GameManager&)  = delete;

    static mem::WeakRef<GameManager> CreateGameManager();

    void Init();
    void Destroy() const;

    void ChangeState(const ApplicationStates state, IApplicationStateInitInfo* initInfo = nullptr);
private:
    GameManager()  = default;
    ~GameManager() = default;

    Scene* mCurrentScene;
    Scene* mPreviousScene;

    static GameManager* mInstance;
};

} // Namespace ignite.