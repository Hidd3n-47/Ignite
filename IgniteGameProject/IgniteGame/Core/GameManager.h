#pragma once

#include <cstdint>

#include "IgniteMem/Core/WeakRef.h"

namespace ignite
{

class Scene;

enum class ApplicationStates : uint8_t
{
    MAIN_MENU,
    GAME,
};

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

    void ChangeState(const ApplicationStates state);
private:
    GameManager()  = default;
    ~GameManager() = default;

    Scene* mCurrentScene;
    Scene* mPreviousScene;

    static GameManager* mInstance;
};

} // Namespace ignite.