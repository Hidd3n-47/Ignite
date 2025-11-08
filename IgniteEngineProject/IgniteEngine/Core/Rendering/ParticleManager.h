#pragma once

namespace ignite
{

class Renderer;
class ParticleSystem;
class ParticleEffect;

struct Particle;
struct ParticleEffectDetails;

class ParticleManager
{
public:
    inline ~ParticleManager() { ClearEffects(); }

    [[nodiscard]] mem::WeakRef<ParticleEffect> AddEffect(const mem::WeakRef<ParticleSystem> system, const mem::WeakRef<ParticleEffectDetails> details);
    void RemoveEffect(const mem::WeakRef<ParticleSystem> system);

    void ClearEffects();

    void Update(const float dt) const;
    void Render(const mem::WeakRef<Renderer> renderer) const;
private:
    std::vector<ParticleEffect*> mEffects;
    std::unordered_map<mem::WeakRef<ParticleSystem>, uint32_t> mSystemToIndex;
    std::unordered_map<uint32_t, mem::WeakRef<ParticleSystem>> mIndexToSystem;

    uint32_t mActiveEffects{ 0 };
};

} // Namespace ignite.