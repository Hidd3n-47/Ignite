#pragma once

#include "IgniteEngine/EC/IComponent.h"

namespace ignite
{

class RaceManagerComponent : public IComponent
{
public:
    RaceManagerComponent(const std::function<void()>& onRaceComplete);

    void PassedStartFinishLine();
private:
    uint32_t mPassedStartLineCount{};
    std::function<void()> mOnRaceComplete;
};

} // Namespace ignite.