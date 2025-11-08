#include "IgnitePch.h"
#include "ParticleManager.h"

#include <cassert>

#include "Defines.h"
#include "Core/Engine.h"
#include "ParticleEffect.h"
#include "EC/Components/ParticleSystem.h"

namespace ignite
{

/* TODO:
 * ================================
 * Ideally this would be better a vector of effects instead of pointer of effects for cache efficiency when iterating.
 * When moving the effect, there is an issue with copying the pointer of particles causing hanging.
 * Additionally, if it is non pointers we get an issue when vector reallocated and moves as the returned weak reference would be
 * invalid after a move. Look at adding a moveable reference that holds references to the weak references, and when moved updates the
 * weak references.
 */
mem::WeakRef<ParticleEffect> ParticleManager::AddEffect(const mem::WeakRef<ParticleSystem> system, const mem::WeakRef<ParticleEffectDetails> details)
{
    mEffects.emplace_back(new ParticleEffect());
    mEffects.back()->InitEffect(details);

    mSystemToIndex[system] = mActiveEffects;
    mIndexToSystem[mActiveEffects++] = system;

    return mem::WeakRef{ mEffects.back() };
}

void ParticleManager::RemoveEffect(const mem::WeakRef<ParticleSystem> system)
{
    const uint32_t deletionIndex = mSystemToIndex[system];

    if (deletionIndex == --mActiveEffects)
    {
        delete mEffects.back();
        mEffects.pop_back();
        mIndexToSystem.clear();
        mSystemToIndex.clear();
        return;
    }

    delete mEffects[deletionIndex];
    mEffects[deletionIndex] = mEffects.back();
    mEffects.pop_back();
    mSystemToIndex[mIndexToSystem[mActiveEffects]] = deletionIndex;
    mIndexToSystem[deletionIndex] = mIndexToSystem[mActiveEffects];
    mSystemToIndex.erase(system);
    mIndexToSystem.erase(mActiveEffects);
}

void ParticleManager::ClearEffects()
{
    for (const ParticleEffect* effect : mEffects)
    {
        delete effect;
    }

    mEffects.clear();
    mIndexToSystem.clear();
    mSystemToIndex.clear();
    mActiveEffects = 0;
}

void ParticleManager::Update(const float dt) const
{
    for (ParticleEffect* effect : mEffects)
    {
        effect->Update(dt);
    }
}

void ParticleManager::Render(const mem::WeakRef<Renderer> renderer) const
{
    for (const ParticleEffect* effect : mEffects)
    {
        effect->Render(renderer);
    }
}

} // Namespace ignite.