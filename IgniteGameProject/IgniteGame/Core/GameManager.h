#pragma once

#include <IgniteMem/Core/WeakHandle.h>

#include "ApplicationState/ApplicationState.h"

#ifdef DEV_CONFIGURATION
#include "Logger/Log.h"
#endif // DEV_CONFIGURATION.

namespace ignite
{
enum class TrophyRanking : uint8_t;

class Scene;
class LevelParser;

class GameManager
{
public:
    [[nodiscard]] inline static mem::WeakHandle<GameManager> Instance() { return mem::WeakHandle{ mInstance }; }

    GameManager(const GameManager&)             = delete;
    GameManager(GameManager&&)                  = delete;
    GameManager& operator=(GameManager&&)       = delete;
    GameManager& operator=(const GameManager&)  = delete;

    static mem::WeakHandle<GameManager> CreateGameManager();

    void Init();
    void Destroy() const;

    void ChangeState(const ApplicationStates state, IApplicationStateInitInfo* initInfo = nullptr);

    inline void SetLevelRankTimes(const float gold, const float silver, const float bronze) { mTimeForGold = gold; mTimeForSilver = silver; mTimeForBronze = bronze; }
    TrophyRanking GetTrophyRanking(const float playerTime) const;

    [[nodiscard]] inline mem::WeakHandle<LevelParser> GetLevelParser() const { return mem::WeakHandle{ mLevelParser }; }

#ifdef DEV_CONFIGURATION
    Log logger{ "IgniteGame" };
#endif // DEV_CONFIGURATION.
private:
    GameManager()  = default;
    ~GameManager() = default;

    Scene* mCurrentScene;
    Scene* mPreviousScene;

    static GameManager* mInstance;

    LevelParser* mLevelParser;

    float mTimeForGold{};
    float mTimeForSilver{};
    float mTimeForBronze{};
};

} // Namespace ignite.