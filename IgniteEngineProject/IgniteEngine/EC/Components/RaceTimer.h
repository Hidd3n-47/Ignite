#pragma once

#include "IgniteEngine/EC/IComponent.h"

namespace ignite
{

class RaceTimer;
class FontRenderer;

class RaceTimer : public IComponent
{
public:
    RaceTimer(const float size);

    void OnComponentAdded(const mem::WeakRef<GameObject> parent) override;
    void OnComponentRemoved() override;

    void Update(const float dt) override;

    void StartTimer();

    [[nodiscard]] inline float Stop() { mTimerStarted = false; return mTimer; }
private:
    mem::WeakRef<FontRenderer> mFontRendererRef;

    bool mTimerStarted = false;

    uint16_t    mId{};
    float       mSize;

    float       mTimer{};
};

} // Namespace ignite.