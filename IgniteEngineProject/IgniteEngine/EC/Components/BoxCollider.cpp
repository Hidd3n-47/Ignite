#include "IgnitePch.h"
#include "BoxCollider.h"

#include "Core/Engine.h"
#include "EC/GameObject.h"
#include "Physics/CollisionHandler.h"

namespace ignite
{

BoxCollider::BoxCollider(const Vec2 dimensionHalfExtents, const bool dynamic, const bool trigger)
    : mDimensionHalfExtents(dimensionHalfExtents)
    , mDynamic(dynamic)
    , mTrigger(trigger)
{
    // Empty.
}

void BoxCollider::OnComponentAdded(const mem::WeakRef<GameObject> parent)
{
    PROFILE_FUNC();

    IComponent::OnComponentAdded(parent);

    if (mDynamic)
    {
        Engine::Instance()->GetCollisionHandler()->AddDynamicBox(mParent);
    }
    else
    {
        Engine::Instance()->GetCollisionHandler()->AddStaticBox(mParent);
    }

#ifdef DEV_CONFIGURATION
    mRenderCommand.transform = mParent->GetComponent<Transform>();
    mRenderCommand.debugSquare = true;
    mRenderCommand.debugSquareHalfExtents = mDimensionHalfExtents;
#endif // DEV_CONFIGURATION.
}

void BoxCollider::OnComponentRemoved()
{
    PROFILE_FUNC();

    if (mDynamic)
    {
        Engine::Instance()->GetCollisionHandler()->RemoveDynamicBox(mParent);
    }
    else
    {
        Engine::Instance()->GetCollisionHandler()->RemoveStaticBox(mParent);
    }
}

#ifdef DEV_CONFIGURATION
void BoxCollider::Render(mem::WeakRef<Renderer> renderer)
{
    PROFILE_FUNC();

    renderer->AddRenderCommand(1000, mem::WeakRef{ &mRenderCommand });
}
#endif // DEV_CONFIGURATION.

void BoxCollider::SetOffset(const Vec2 offset)
{
    PROFILE_FUNC();

    mOffset = offset;

#ifdef DEV_CONFIGURATION
    mRenderCommand.offset = offset;
#endif // DEV_CONFIGURATION.
}

} // Namespace ignite.