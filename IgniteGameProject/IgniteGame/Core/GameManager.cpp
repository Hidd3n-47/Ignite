#include "GameManager.h"

#include <IgniteEngine/Core/Engine.h>
#include <IgniteEngine/EC/Scene.h>

#include "Src/Defines.h"

#include "ApplicationState/GameApplicationState.h"
#include "ApplicationState/MainMenuApplicationState.h"

namespace ignite
{

GameManager* GameManager::mInstance = nullptr;

mem::WeakRef<GameManager> GameManager::CreateGameManager()
{
    if (mInstance)
    {
        GAME_ERROR("Failed to create game manager as it has already been created!");
        GAME_BREAK();

        return mem::WeakRef{ mInstance };
    }

    mInstance = new GameManager();
    GAME_INFO("Successfully created Ignite Game.");

    return mem::WeakRef{ mInstance };
}

void GameManager::Init()
{
    ChangeState(ApplicationStates::MAIN_MENU);
}

void GameManager::Destroy() const
{
    delete mInstance;
    mInstance = nullptr;

    GAME_INFO("Successfully destroyed Ignite Game.");
}

void GameManager::ChangeState(const ApplicationStates state)
{
    delete mPreviousScene;

    mPreviousScene = mCurrentScene;

    switch (state)
    {
    case ApplicationStates::MAIN_MENU:
        GAME_LOG("Updated Application State: MAIN_MENU");
        mCurrentScene = new MainMenuApplicationState();
        break;
    case ApplicationStates::GAME:
        GAME_LOG("Updated Application State: GAME");
        mCurrentScene = new GameApplicationState();
        break;
    default:
        GAME_ERROR("Trying to change game state to an unhandled state with ID: {}", static_cast<uint32_t>(state));
        break;
    }

    Engine::Instance()->SetSceneToChangeTo(mem::WeakRef{ mCurrentScene });
}

} // Namespace ignite.