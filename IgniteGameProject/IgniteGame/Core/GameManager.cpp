#include "GameManager.h"

#include <IgniteEngine/Core/Engine.h>
#include <IgniteEngine/EC/Scene.h>

#include "TrophyRanking.h"
#include "Src/Defines.h"
#include "Level/LevelParser.h"

#include "ApplicationState/GameApplicationState.h"
#include "ApplicationState/MainMenuApplicationState.h"
#include "ApplicationState/LevelSelectApplicationState.h"
#include "ApplicationState/RewardsApplicationState.h"

namespace ignite
{

GameManager* GameManager::mInstance = nullptr;

mem::WeakHandle<GameManager> GameManager::CreateGameManager()
{
    if (mInstance)
    {
        GAME_ERROR("Failed to create game manager as it has already been created!");
        GAME_BREAK();

        return mem::WeakHandle{ mInstance };
    }

    mInstance = new GameManager();
    GAME_INFO("Successfully created Ignite Game.");

    return mem::WeakHandle{ mInstance };
}

void GameManager::Init()
{
    PROFILE_FUNC();
    mLevelParser = new LevelParser();

    mLevelParser->Init();

    ChangeState(ApplicationStates::MAIN_MENU);
}

void GameManager::Destroy() const
{
    PROFILE_FUNC();

    delete mPreviousScene;
    delete mCurrentScene;

    mLevelParser->Destroy();
    delete mLevelParser;

    GAME_INFO("Successfully destroyed Ignite Game.");

    delete mInstance;
    mInstance = nullptr;
}

void GameManager::ChangeState(const ApplicationStates state, IApplicationStateInitInfo* initInfo)
{
    PROFILE_FUNC();

    delete mPreviousScene;

    mPreviousScene = mCurrentScene;

    switch (state)
    {
    case ApplicationStates::MAIN_MENU:
        GAME_LOG("Updated Application State: MAIN_MENU");
        mCurrentScene = new MainMenuApplicationState();
        break;
    case ApplicationStates::LEVEL_SELECT:
        GAME_LOG("Updated Application State: LEVEL_SELECT");
        mCurrentScene = new LevelSelectApplicationState();
        break;
    case ApplicationStates::GAME:
    {
        GAME_LOG("Updated Application State: GAME");
        GameApplicationStateInitInfo* info = dynamic_cast<GameApplicationStateInitInfo*>(initInfo);
        mCurrentScene = new GameApplicationState(mem::WeakHandle{ info });
        break;
    }
    case ApplicationStates::REWARDS:
    {
        GAME_LOG("Updated Application State: REWARDS");
        RewardsApplicationStateInitInfo* info = dynamic_cast<RewardsApplicationStateInitInfo*>(initInfo);
        mCurrentScene = new RewardsApplicationState(mem::WeakHandle{ info });
        break;
    }
    default:
        GAME_ERROR("Trying to change game state to an unhandled state with ID: {}", static_cast<uint32_t>(state));
        break;
    }

    delete initInfo;

    Engine::Instance()->SetSceneToChangeTo(mem::WeakHandle{ mCurrentScene });
}

TrophyRanking GameManager::GetTrophyRanking(const float playerTime) const
{
    PROFILE_FUNC();

    if (playerTime > mTimeForBronze)
    {
        return TrophyRanking::NONE;
    }

    if (playerTime > mTimeForSilver)
    {
        return TrophyRanking::BRONZE;
    }

    if (playerTime > mTimeForGold)
    {
        return TrophyRanking::SILVER;
    }

    return TrophyRanking::GOLD;
}

} // Namespace ignite.