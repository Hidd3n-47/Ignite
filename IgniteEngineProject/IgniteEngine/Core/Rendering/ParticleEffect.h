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

    void InitEffect(const mem::WeakHandle<ParticleEffectDetails> details, const mem::WeakHandle<Transform> parentTransform);

    void Update(const float dt);
    void Render(mem::WeakHandle<Renderer> renderer) const;

    void Emmit(const bool emmit = true);
private:
    Particle* mParticles = nullptr;
    mem::WeakHandle<Transform> mParentTransform;
    mem::WeakHandle<ParticleEffectDetails> mEffectDetails;

    uint16_t mTextureId{};

    float mSpawnTimer   { 0.0f  };
    bool  mEmitting     { false };

    uint32_t mSpawnIndex {};
};

} // Namespace ignite.