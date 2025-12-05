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

    if (!mCanMove)
    {
        return;
    }

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

    const bool accelerating = Engine::Instance()->GetInputManager()->IsKeyDown(Keycode::KEY_W);
    const bool decelerating = Engine::Instance()->GetInputManager()->IsKeyDown(Keycode::KEY_S);
    force += accelerating ? forwardVector * mAccelerationForce : Vec2{};
    force += decelerating ? forwardVector * mAccelerationForce * -0.5f : Vec2{};

    // Drag.
    const float speedSq = mVelocity.MagnitudeSquared();
    if (speedSq > 0) 
    {
        force += mVelocity / sqrt(speedSq) * -(mDragCoefficient * speedSq);
    }

    const Vec2 acceleration = force / mMass;

    mVelocity += acceleration;
    mTransform->translation += mVelocity * dt;

    float steer = 0.0f;
    if (Engine::Instance()->GetInputManager()->IsKeyDown(Keycode::KEY_D))
    {
        steer += 1.0f;
    }
    if (Engine::Instance()->GetInputManager()->IsKeyDown(Keycode::KEY_A))
    {
        steer -= 1.0f;
    }

    if (decelerating)
    {
        steer *= -1.0f;
    }

    const float turnTorque = steer * mVelocity.Magnitude() * 500.0f;
    mAngularVelocity += turnTorque / mInertia;
    mAngularVelocity *= 0.85f;
    mTransform->rotation += mAngularVelocity * dt;
}

} // Namespace ignite.