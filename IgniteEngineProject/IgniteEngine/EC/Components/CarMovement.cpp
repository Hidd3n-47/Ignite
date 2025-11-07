#include "IgnitePch.h"
#include "CarMovement.h"

#include "Core/Engine.h"
#include "Core/Input/Keycode.h"
#include "Core/Input/InputManager.h"
#include "EC/GameObject.h"

#include "EC/Components/Transform.h"
#include "Math/Math.h"
#include "Math/Mat2.h"

namespace ignite
{

void CarMovement::OnComponentAdded(const mem::WeakRef<GameObject> parent)
{
    IComponent::OnComponentAdded(parent);

    mTransform       = mParent->GetComponent<Transform>();
    mInputManagerRef = Engine::Instance()->GetInputManager();
}

void CarMovement::Update(const float dt)
{
    if (!mMovementEnabled)
    {
        return;
    }

    const float rotation = mTransform->rotation;
    const float cosRot   = std::cos(Math::DegToRads(rotation));
    const float sinRot   = std::sin(Math::DegToRads(rotation));
    const Mat2 rotMat
    {
        .m11 =  cosRot,
        .m12 = -sinRot,
        .m21 =  sinRot,
        .m22 =  cosRot
    };
    const Vec2 forwardVector = rotMat * Vec2{ 1.0f, 0.0f };

    Vec2 direction;
    if (mInputManagerRef->IsKeyDown(Keycode::KEY_W))
    {
        direction += Vec2{ 1.0f, 0.0f };
    }
    if (mInputManagerRef->IsKeyDown(Keycode::KEY_S))
    {
        direction -= Vec2{ 1.0f, 0.0f };
    }
    if (mInputManagerRef->IsKeyDown(Keycode::KEY_D))
    {
        direction += Vec2{ 0.0f, 1.0f };
    }
    if (mInputManagerRef->IsKeyDown(Keycode::KEY_A))
    {
        direction -= Vec2{ 0.0f, 1.0f };
    }

    mTransform->translation += forwardVector * direction.x * mSpeed * dt;
    mTransform->rotation    += direction.y * mSpeed * mTurnSpeed * direction.x * dt;
}

} // Namespace ignite.