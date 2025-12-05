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

void CarMovement::OnComponentAdded(const mem::WeakHandle<GameObject> parent)
{
    PROFILE_FUNC();

    IComponent::OnComponentAdded(parent);

    mTransform       = mParent->GetComponent<Transform>();
    mInputManagerRef = Engine::Instance()->GetInputManager();
}

void CarMovement::Update(const float dt)
{
    PROFILE_FUNC();

    if (!mMovementEnabled)
    {
        return;
    }

    Vec2 direction;
    if (mInputManagerRef->IsKeyDown(Keycode::KEY_W))
    {
        direction += Vec2{ 1.0f, 0.0f };
    }
    if (mInputManagerRef->IsKeyDown(Keycode::KEY_S))
    {
        direction -= Vec2{ 1.0f, 0.0f };
    }

    mMoving = direction.x != 0.0f;
}

} // Namespace ignite.