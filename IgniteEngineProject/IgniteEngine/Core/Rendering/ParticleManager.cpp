#include "IgnitePch.h"
#include "ParticleManager.h"

#include <cassert>

#include "Defines.h"
#include "Core/Engine.h"
#include "ParticleEffect.h"
#include "EC/Components/ParticleSystem.h"

namespace ignite
{
ParticleManager::ParticleManager()
{
    /* TODO:
     * ================================
     * This could cause a big problem since any re-allocation and move in the vector would result in all the returned
     * Weak references to be invalid, however, I would like effects to be contiguous as we iterate over them every frame.
     * In order to keep this here we can manually reserve 10 for now, but should look at adding mem::MoveableRef.
     * This Ref will store all weak references that need to be updated and in the move constructor update all the references.
     * Note we need to consider how weak references are just a wrapper, so normal use doesn't care about copy operations
     * Which would obviously be an issue if the moveable ref keeps a pointer to that weak ref.
     */
    mEffects.reserve(MAX_CAPACITY);
}

mem::WeakRef<ParticleEffect> ParticleManager::AddEffect(const mem::WeakRef<ParticleSystem> system, const mem::WeakRef<ParticleEffectDetails> details)
{
    if (mActiveEffects + 1 >= MAX_CAPACITY)
    {
        assert(false);
        DEBUG_BREAK();
    }

    mEffects.emplace_back();
    mEffects.back().InitEffect(details);

    mSystemToIndex[system] = mActiveEffects;
    mIndexToSystem[mActiveEffects++] = system;

    return mem::WeakRef{ &mEffects.back() };
}

void ParticleManager::RemoveEffect(const mem::WeakRef<ParticleSystem> system)
{
    const uint32_t deletionIndex = mSystemToIndex[system];

    if (deletionIndex == --mActiveEffects)
    {
        mEffects.pop_back();
        return;
    }

    mEffects[deletionIndex] = mEffects.back();
    mSystemToIndex[mIndexToSystem[mActiveEffects]] = deletionIndex;
    mSystemToIndex.erase(system);
    mIndexToSystem.erase(mActiveEffects);
}

void ParticleManager::ClearEffects()
{
    mEffects.clear();
    mIndexToSystem.clear();
    mSystemToIndex.clear();
    mActiveEffects = 0;
}

void ParticleManager::Update(const float dt)
{
    for (ParticleEffect& effect : mEffects)
    {
        effect.Update(dt);
    }
}

void ParticleManager::Render(mem::WeakRef<Renderer> renderer) const
{
    for (const ParticleEffect& effect : mEffects)
    {
        effect.Render(renderer);
    }
}

} // Namespace ignite.