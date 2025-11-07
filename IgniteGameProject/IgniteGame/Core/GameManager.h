#pragma once

#include <IgniteMem/Core/WeakRef.h>

#include "ApplicationState/ApplicationState.h"

namespace ignite
{
enum class TrophyRanking : uint8_t;

class Scene;
class LevelParser;

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

    inline void SetLevelRankTimes(const float gold, const float silver, const float bronze) { mTimeForGold = gold; mTimeForSilver = silver; mTimeForBronze = bronze; }
    TrophyRanking GetTrophyRanking(const float playerTime) const;

    [[nodiscard]] inline mem::WeakRef<LevelParser> GetLevelParser() const { return mem::WeakRef{ mLevelParser }; }
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