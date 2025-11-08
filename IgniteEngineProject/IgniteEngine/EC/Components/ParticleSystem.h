#pragma once

#include "IgniteEngine/EC/IComponent.h"

#include "IgniteEngine/Core/Rendering/ParticleEffectDetails.h"

namespace ignite
{

struct Particle;
class InputManager;
class ParticleEffect;

class ParticleSystem : public IComponent
{
public:
    ParticleSystem(ParticleEffectDetails&& particleEffectDetails);

    void OnComponentAdded(const mem::WeakRef<GameObject> parent) override;
    void OnComponentRemoved() override;
private:
    ParticleEffectDetails mParticleEffectDetails;
    mem::WeakRef<ParticleEffect> mEffect;

};

} // Namespace ignite.