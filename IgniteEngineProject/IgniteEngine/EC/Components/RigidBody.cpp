#include "IgnitePch.h"
#include "RigidBody.h"

#include "EC/GameObject.h"
#include "Core/Engine.h"
#include "Core/Input/InputManager.h"
#include "Core/Input/Keycode.h"
#include "EC/Components/CarMovement.h"
#include "Math/Mat2.h"
#include "Math/Math.h"

namespace ignite
{

void RigidBody::OnComponentAdded(const mem::WeakHandle<GameObject> parent)
{
    PROFILE_FUNC();

    IComponent::OnComponentAdded(parent);
    mTransform = mParent->GetComponent<Transform>();
}

void RigidBody::OnComponentRemoved()
{
    
}

void RigidBody::Update(const float dt)
{
    PROFILE_FUNC();

    const float rotation = mTransform->rotation;
    const float cosRot = std::cos(Math::DegToRads(rotation));
    const float sinRot = std::sin(Math::DegToRads(rotation));
    const Mat2 rotMat
    {
        .m11 = cosRot,
        .m12 = -sinRot,
        .m21 = sinRot,
        .m22 = cosRot
    };
    const Vec2 forwardVector = rotMat * Vec2{ 1.0f, 0.0f };

    Vec2 force;

    // Force from movement.
    bool accelerating = Engine::Instance()->GetInputManager()->IsKeyDown(Keycode::KEY_W);
    force += accelerating ? forwardVector * mAccelerationForce : Vec2{};

    // Drag
    float speedSq = mVelocity.MagnitudeSquared();
    if (speedSq > 0) 
    {
        force += mVelocity / sqrt(speedSq) * -(mDragCoefficient * speedSq);
    }

    Vec2 acceleration = force / mMass;

    mVelocity += acceleration;
    mTransform->translation += mVelocity * dt;

    // Steering torque
    float steer = 0.0f;
    if (Engine::Instance()->GetInputManager()->IsKeyDown(Keycode::KEY_D))
        steer += 1.0f;
    if (Engine::Instance()->GetInputManager()->IsKeyDown(Keycode::KEY_A))
        steer -= 1.0f;

    float turnTorque = steer * mVelocity.Magnitude() * 500.0f;
    mAngularVelocity += turnTorque / mInertia;
    mAngularVelocity *= 0.85f;
    mTransform->rotation += mAngularVelocity * dt;
}

} // Namespace ignite.