#pragma once

namespace ignite
{

struct ParticleEffectDetails
{
    std::string textureFilepath{};
    uint32_t    textureLayer = 0;

    uint32_t numberOfParticles = 1;

    mem::WeakRef<Vec2> position{};
    Vec2 minPositionOffset{};
    Vec2 maxPositionOffset{};

    float minLifetime = 1.0f;
    float maxLifetime = 1.0f;

    float minScale = 1.0f;
    float maxScale = 1.0f;

    float particleSpawnInterval = 1.0f;
};

} // Namespace ignite.