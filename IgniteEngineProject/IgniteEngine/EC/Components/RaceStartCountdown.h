#pragma once

#include "IgniteEngine/EC/IComponent.h"

#include "IgniteEngine/Core/Rendering/RenderCommand.h"

namespace ignite
{

class RaceStartCountdown : public IComponent
{
public:
    RaceStartCountdown(const float countdownTime, const std::function<void()>& onTimerCompletedCallback);

    inline void StartTimer() { mTimer = mCountdownTimeMax; mTimerRunning = true; }

    void Update(const float dt) override;
private:
    float mCountdownTimeMax;
    float mTimer;
    bool  mTimerRunning = false;

    std::function<void()> mOnTimerCompletedEvent;
};

} // Namespace ignite.