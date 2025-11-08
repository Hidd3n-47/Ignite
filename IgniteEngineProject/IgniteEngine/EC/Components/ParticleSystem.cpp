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

void ParticleSystem::OnComponentAdded(const mem::WeakRef<GameObject> parent)
{
    mEffect = Engine::Instance()->GetParticleManager()->AddEffect(mem::WeakRef{ this }, mem::WeakRef{ &mParticleEffectDetails });
    mEffect->Emmit();
}

void ParticleSystem::OnComponentRemoved()
{
    Engine::Instance()->GetParticleManager()->RemoveEffect(mem::WeakRef{ this });
}

} // Namespace ignite.