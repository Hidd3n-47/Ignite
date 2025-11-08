#pragma once

namespace ignite
{

class Transform;
struct Particle;
struct ParticleEffectDetails;

class Renderer;

class ParticleEffect
{
public:
    ~ParticleEffect();

    void InitEffect(const mem::WeakRef<ParticleEffectDetails> details, const mem::WeakRef<Transform> parentTransform);

    void Update(const float dt);
    void Render(mem::WeakRef<Renderer> renderer) const;

    void Emmit(const bool emmit = true);
private:
    Particle* mParticles = nullptr;
    mem::WeakRef<Transform> mParentTransform;
    mem::WeakRef<ParticleEffectDetails> mEffectDetails;

    uint16_t mTextureId{};

    float mSpawnTimer   { 0.0f  };
    bool  mEmitting     { false };

    uint32_t mSpawnIndex {};
};

} // Namespace ignite.