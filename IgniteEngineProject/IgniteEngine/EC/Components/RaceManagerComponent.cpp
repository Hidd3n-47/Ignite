#include "IgnitePch.h"
#include "RaceManagerComponent.h"

namespace ignite
{

RaceManagerComponent::RaceManagerComponent(const std::function<void()>& onRaceComplete)
    : mOnRaceComplete(onRaceComplete)
{
    // Empty.
}

void RaceManagerComponent::PassedStartFinishLine()
{
    if (++mPassedStartLineCount >= 2 && mOnRaceComplete)
    {
        mOnRaceComplete();
    }
}

} // Namespace ignite.