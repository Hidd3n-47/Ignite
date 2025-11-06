#include "IgnitePch.h"
#include "RaceStartCountdown.h"

#include "UiText.h"
#include "EC/GameObject.h"

namespace ignite
{

RaceStartCountdown::RaceStartCountdown(const mem::WeakRef<UiText> uiTextComponent, const float countdownTime, const std::function<void()>& onTimerCompletedCallback)
    : mUiTextComponent(uiTextComponent)
    , mCountdownTimeMax(countdownTime)
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

        mUiTextComponent->GetParent()->RemoveComponent<UiText>();

        if (mOnTimerCompletedEvent)
        {
            mOnTimerCompletedEvent();
        }

        return;
    }

    // Do this last so that we don't waste computation on creating the font if the timer has run out.
    mUiTextComponent->SetText(std::to_string(static_cast<uint32_t>(mTimer)));
}

} // Namespace ignite.