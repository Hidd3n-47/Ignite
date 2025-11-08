#pragma once

#include "IgniteEngine/EC/IComponent.h"

namespace ignite
{

class Transform;

class RigidBody : public IComponent
{
public:
    void OnComponentAdded(const mem::WeakRef<GameObject> parent) override;
    void OnComponentRemoved() override;

    void Update(const float dt) override;
private:
    Vec2 mVelocity;
    float mMass{ 100.0f };
    float mAccelerationForce{ 50.0f };
    float mDragCoefficient = 3.0f;
    float mAngularVelocity = 0.0f;
    float mInertia = 100.0f;

    mem::WeakRef<Transform> mTransform;

};

} // Namespace ignite.