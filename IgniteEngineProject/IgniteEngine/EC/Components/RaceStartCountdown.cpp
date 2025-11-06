#include "IgnitePch.h"
#include "RaceStartCountdown.h"

namespace ignite
{

RaceStartCountdown::RaceStartCountdown(const float countdownTime, const std::function<void()>& onTimerCompletedCallback)
    : mCountdownTimeMax(countdownTime)
    , mTimer(countdownTime)
    , mOnTimerCompletedEvent(onTimerCompletedCallback)
{
    // Empty.
}

void RaceStartCountdown::Update(const float dt)
{
    if (!mTimerRunning)
    {
        return;
    }

    mTimer -= dt;

    if (mTimer <= 0.0f)
    {
        mTimerRunning = false;

        if (mOnTimerCompletedEvent)
        {
            mOnTimerCompletedEvent();
        }
    }
}

} // Namespace ignite.