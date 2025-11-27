#include "IgnitePch.h"
#include "RaceTimer.h"

#include "Core/Engine.h"
#include "EC/GameObject.h"
#include "Core/Rendering/FontRenderer.h"
#include "Math/Math.h"

namespace ignite
{

RaceTimer::RaceTimer(const float size)
    : mSize(size)
{
    mFontRendererRef = Engine::Instance()->GetFontRenderer();
}

void RaceTimer::OnComponentAdded(const mem::WeakRef<GameObject> parent)
{
    PROFILE_FUNC();

    IComponent::OnComponentAdded(parent);

    mId = mFontRendererRef->CreateFont("Assets/Fonts/ThaleahFat.ttf", mSize, "0.0", mParent->GetComponent<Transform>());
}

void RaceTimer::OnComponentRemoved()
{
    PROFILE_FUNC();

    mFontRendererRef->RemoveFont(mId);
}

void RaceTimer::Update(const float dt)
{
    PROFILE_FUNC();

    if (!mTimerStarted)
    {
        return;
    }

    mTimer += dt;

    const float value = static_cast<float>(static_cast<uint32_t>(mTimer * 10.0f)) / 10.0f;
    if (Math::IsEqualTo(value, mTimerValue))
    {
        mTimerValue = value;
        mFontRendererRef->UpdateFont(mId, std::format("{:1}", mTimerValue));
    }
}

void RaceTimer::StartTimer()
{
    PROFILE_FUNC();

    mTimer = 0.0f;

    mTimerStarted = true;
}

} // Namespace ignite.