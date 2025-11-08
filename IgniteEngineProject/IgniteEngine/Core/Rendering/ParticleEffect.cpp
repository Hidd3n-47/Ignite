#include "IgnitePch.h"
#include "ParticleEffect.h"

#include "Particle.h"
#include "Core/Engine.h"
#include "Core/Random/Random.h"
#include "EC/Components/ParticleSystem.h"
#include "Core/Rendering/TextureManager.h"

namespace ignite
{

ParticleEffect::~ParticleEffect()
{
    delete[] mParticles;

    Engine::Instance()->GetTextureManager()->RemoveTexture(mTextureId);
}

void ParticleEffect::InitEffect(const mem::WeakRef<ParticleEffectDetails> details, const mem::WeakRef<Transform> parentTransform)
{
    mParticles = new Particle[details->numberOfParticles];

    mEffectDetails = details;
    mParentTransform = parentTransform;

    Texture particleTexture;
    Engine::Instance()->GetTextureManager()->Load(particleTexture, details->textureFilepath);
    mTextureId    = particleTexture.id;

    for (uint32_t i{ 0 }; i < details->numberOfParticles; ++i)
    {
        mParticles[i].renderCommand.texture = particleTexture;
    }
}

void ParticleEffect::Update(const float dt)
{
    // Spawn particles.
    if (mEmitting)
    {
        mSpawnTimer -= dt;

        if (mSpawnTimer < 0.0f)
        {
            mSpawnTimer = mEffectDetails->particleSpawnInterval;
            mParticles[mSpawnIndex].transform.translation = *mEffectDetails->position;

            Vec2 offset;
            offset.x = Random::GetRandomBetween(mEffectDetails->minPositionOffset.x, mEffectDetails->maxPositionOffset.x);
            offset.y = Random::GetRandomBetween(mEffectDetails->minPositionOffset.y, mEffectDetails->maxPositionOffset.y);
            mParticles[mSpawnIndex].renderCommand.offset = offset;

            mParticles[mSpawnIndex].lifetime = Random::GetRandomBetween(mEffectDetails->minLifetime, mEffectDetails->maxLifetime);
            mParticles[mSpawnIndex].initialLifetime = mParticles[mSpawnIndex].lifetime;

            mParticles[mSpawnIndex].transform.scale = Vec2{ Random::GetRandomBetween(mEffectDetails->minScale, mEffectDetails->maxScale) };

            mSpawnIndex = (mSpawnIndex + 1) % mEffectDetails->numberOfParticles;
        }
    }

    // Update particles.
    for (uint32_t i{ 0 }; i < mEffectDetails->numberOfParticles; ++i)
    {
        if (mParticles[i].lifetime <= 0.0f)
        {
            continue;
        }

        mParticles[i].lifetime -= dt;

        mParticles[i].renderCommand.alpha = static_cast<uint8_t>(std::max(0.0f, mParticles[i].lifetime) / mParticles[i].initialLifetime * 255);
    }
}

void ParticleEffect::Render(mem::WeakRef<Renderer> renderer) const
{
    for (uint32_t i{ 0 }; i < mEffectDetails->numberOfParticles; ++i)
    {
        if (mParticles[i].lifetime > 0)
        {
            mParticles[i].renderCommand.transform->rotation = mParentTransform->rotation;
            renderer->AddRenderCommand(mEffectDetails->textureLayer, mem::WeakRef{ &mParticles[i].renderCommand });
        }
    }
}

void ParticleEffect::Emmit(const bool emmit)
{
    mEmitting   = emmit;
    //mSpawnTimer = mEffectDetails->particleSpawnInterval;
}

} // Namespace ignite.