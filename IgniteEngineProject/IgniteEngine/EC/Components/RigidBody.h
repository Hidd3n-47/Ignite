#pragma once

#include "IgniteEngine/EC/IComponent.h"
#include "IgniteEngine/EC/IUpdateableComponent.h"

namespace ignite
{

class Transform;

class RigidBody : public IComponent, public IUpdateableComponent
{
public:
    void OnComponentAdded(const mem::WeakHandle<GameObject> parent) override;
    void OnComponentRemoved() override;

    void Update(const float dt) override;

    inline void SetCanMove(const bool canMove) { mCanMove = canMove; }
private:
    Vec2 mVelocity;
    float mMass{ 100.0f };
    float mAccelerationForce{ 50.0f };
    float mDragCoefficient = 3.0f;
    float mAngularVelocity = 0.0f;
    float mInertia = 100.0f;
    bool  mCanMove = true;

    mem::WeakHandle<Transform> mTransform;

};

} // Namespace ignite.