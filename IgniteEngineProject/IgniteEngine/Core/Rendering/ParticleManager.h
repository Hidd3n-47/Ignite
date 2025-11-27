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

    [[nodiscard]] mem::WeakHandle<ParticleEffect> AddEffect(const mem::WeakHandle<ParticleSystem> system, const mem::WeakHandle<ParticleEffectDetails> details);
    void RemoveEffect(const mem::WeakHandle<ParticleSystem> system);

    void ClearEffects();

    void Update(const float dt) const;
    void Render(const mem::WeakHandle<Renderer> renderer) const;
private:
    std::vector<ParticleEffect*> mEffects;
    std::unordered_map<mem::WeakHandle<ParticleSystem>, uint32_t> mSystemToIndex;
    std::unordered_map<uint32_t, mem::WeakHandle<ParticleSystem>> mIndexToSystem;

    uint32_t mActiveEffects{ 0 };
};

} // Namespace ignite.