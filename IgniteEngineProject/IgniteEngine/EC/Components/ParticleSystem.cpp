#include "IgnitePch.h"
#include "ParticleSystem.h"

#include "Core/Engine.h"
#include "Core/Rendering/ParticleEffect.h"
#include "Core/Rendering/ParticleManager.h"

namespace ignite
{

ParticleSystem::ParticleSystem(ParticleEffectDetails&& particleEffectDetails)
    : mParticleEffectDetails(std::move(particleEffectDetails))
{
    // Empty.
}

void ParticleSystem::OnComponentAdded(const mem::WeakHandle<GameObject> parent)
{
    PROFILE_FUNC();

    IComponent::OnComponentAdded(parent);

    mEffect = Engine::Instance()->GetParticleManager()->AddEffect(mem::WeakHandle{ this }, mem::WeakHandle{ &mParticleEffectDetails });
    mEffect->Emmit();
}

void ParticleSystem::OnComponentRemoved()
{
    PROFILE_FUNC();

    Engine::Instance()->GetParticleManager()->RemoveEffect(mem::WeakHandle{ this });
}

void ParticleSystem::Emmit(const bool emmit)
{
    PROFILE_FUNC();

    mEffect->Emmit(emmit);
}

} // Namespace ignite.