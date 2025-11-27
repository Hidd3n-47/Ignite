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

    void OnComponentAdded(const mem::WeakHandle<GameObject> parent) override;
    void OnComponentRemoved() override;

    void Emmit(const bool emmit = true);
private:
    ParticleEffectDetails mParticleEffectDetails;
    mem::WeakHandle<ParticleEffect> mEffect;

};

} // Namespace ignite.