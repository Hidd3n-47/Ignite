#pragma once

#include "IgniteEngine/Core/Rendering/ParticleEffect.h"

namespace ignite
{

class Renderer;
class ParticleSystem;

struct Particle;
struct ParticleEffectDetails;

class ParticleManager
{
public:
    ParticleManager();
    inline ~ParticleManager() { ClearEffects(); }

    [[nodiscard]] mem::WeakRef<ParticleEffect> AddEffect(const mem::WeakRef<ParticleSystem> system, const mem::WeakRef<ParticleEffectDetails> details);
    void RemoveEffect(const mem::WeakRef<ParticleSystem> system);

    void ClearEffects();

    void Update(const float dt);
    void Render(mem::WeakRef<Renderer> renderer) const;

    inline static constexpr uint32_t MAX_CAPACITY = 10;
private:
    std::vector<ParticleEffect> mEffects;
    std::unordered_map<mem::WeakRef<ParticleSystem>, uint32_t> mSystemToIndex;
    std::unordered_map<uint32_t, mem::WeakRef<ParticleSystem>> mIndexToSystem;

    uint32_t mActiveEffects{ 0 };
};

} // Namespace ignite.