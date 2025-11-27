#pragma once

#include <IgniteEngine/EC/Scene.h>

#include "ApplicationState.h"

#include "Core/TrophyRanking.h"

namespace ignite
{

class RewardsApplicationStateInitInfo : public IApplicationStateInitInfo
{
public:
    RewardsApplicationStateInitInfo(const TrophyRanking ranking) : ranking(ranking) {}

    TrophyRanking ranking;
};


class RewardsApplicationState : public Scene
{
public:
    RewardsApplicationState(mem::WeakHandle<RewardsApplicationStateInitInfo> initInfo);

    void InitScene() override;

private:
    TrophyRanking mPlayerRanking;

    mem::WeakHandle<GameObject> mTextObject;
};

} // Namespace ignite.