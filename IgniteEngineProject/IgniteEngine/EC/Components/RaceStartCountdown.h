#pragma once

#include "IgniteEngine/EC/IComponent.h"
#include "IgniteEngine/EC/IUpdateableComponent.h"

namespace ignite
{

class UiText;

class RaceStartCountdown : public IComponent, public IUpdateableComponent
{
public:
    RaceStartCountdown(const mem::WeakHandle<UiText> uiTextComponent, const float countdownTime, const std::function<void()>& onTimerCompletedCallback);

    inline void StartTimer() { mTimer = mCountdownTimeMax; mTimerRunning = true; }

    void Update(const float dt) override;
private:
    mem::WeakHandle<UiText> mUiTextComponent;
    float mCountdownTimeMax;
    float mTimer;
    bool  mTimerRunning = false;

    std::function<void()> mOnTimerCompletedEvent;
};

} // Namespace ignite.