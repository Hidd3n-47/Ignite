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
    if (mDynamic)
    {
        Engine::Instance()->GetCollisionHandler()->RemoveDynamicBox(mParent);
    }
    else
    {
        Engine::Instance()->GetCollisionHandler()->RemoveStaticBox(mParent);
    }
}

void BoxCollider::Update(const float dt)
{

}

#ifdef DEV_CONFIGURATION
void BoxCollider::Render(mem::WeakRef<Renderer> renderer)
{
    renderer->AddRenderCommand(1000, mem::WeakRef{ &mRenderCommand });
}
#endif // DEV_CONFIGURATION.

} // Namespace ignite.