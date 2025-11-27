#include "IgnitePch.h"
#include "CollisionHandler.h"

#include <IgniteMem/Core/WeakHandleHash.h>

#include "Defines.h"
#include "EC/GameObject.h"
#include "EC/Components/Transform.h"
#include "EC/Components/BoxCollider.h"

#include "Math/Math.h"

namespace ignite
{
void CollisionHandler::Update()
{
    PROFILE_FUNC();

    mTriggeredThisFrame.clear();

    for (const mem::WeakHandle<GameObject> dynamicBox1 : mDynamicBoxColliders)
    {
        for (const mem::WeakHandle<GameObject> dynamicBox2 : mDynamicBoxColliders)
        {
            if (dynamicBox1 == dynamicBox2)
            {
                continue;
            }

            CheckCollisionBetweenBoxes(dynamicBox1, dynamicBox2, true);
        }
    }

    for (const mem::WeakHandle<GameObject> dynamicBox1 : mDynamicBoxColliders)
    {
        for (const mem::WeakHandle<GameObject> staticBox : mStaticBoxColliders)
        {
            CheckCollisionBetweenBoxes(dynamicBox1, staticBox, false);
        }
    }

    for (auto& [gameObject, triggerInfo] : mTriggeredThisFrame)
    {
        if (mTriggeredPrevFrame.contains(gameObject) && mTriggeredPrevFrame[gameObject].otherBody == triggerInfo.otherBody)
        {
            continue;
        }

        triggerInfo.bodyCollider->OnTriggerEnter(triggerInfo.otherBody);
    }

    mTriggeredPrevFrame = mTriggeredThisFrame;
}

void CollisionHandler::AddDynamicBox(const mem::WeakHandle<GameObject> box)
{
    PROFILE_FUNC();

    mDynamicBoxColliders.emplace_back(box);
}

void CollisionHandler::AddStaticBox(const mem::WeakHandle<GameObject> box)
{
    PROFILE_FUNC();

    mStaticBoxColliders.emplace_back(box);
}

void CollisionHandler::RemoveDynamicBox(const mem::WeakHandle<GameObject> box)
{
    PROFILE_FUNC();

    for (auto it{ mDynamicBoxColliders.begin() }; it != mDynamicBoxColliders.end(); ++it)
    {
        if (*it == box)
        {
            mDynamicBoxColliders.erase(it);
            return;
        }
    }
}

void CollisionHandler::RemoveStaticBox(const mem::WeakHandle<GameObject> box)
{
    PROFILE_FUNC();

    for (auto it{ mStaticBoxColliders.begin() }; it != mStaticBoxColliders.end(); ++it)
    {
        if (*it == box)
        {
            mStaticBoxColliders.erase(it);
            return;
        }
    }
}

void CollisionHandler::CheckCollisionBetweenBoxes(mem::WeakHandle<GameObject> box1, mem::WeakHandle<GameObject> box2, const bool pushBothBodies)
{
    PROFILE_FUNC();

    mem::WeakHandle<Transform>   box1Transform = box1->GetComponent<Transform>();
    mem::WeakHandle<BoxCollider> box1Collider  = box1->GetComponent<BoxCollider>();

    const Vec2 b1Centre = box1Transform->translation + box1Collider->GetOffset();
    const Vec2 box1HalfExtents = box1Collider->GetDimensionHalfExtents();

    mem::WeakHandle<Transform>   box2Transform = box2->GetComponent<Transform>();
    mem::WeakHandle<BoxCollider> box2Collider  = box2->GetComponent<BoxCollider>();

    const Vec2 b2Centre = box2Transform->translation + box2Collider->GetOffset();
    const Vec2 box2HalfExtents = box2Collider->GetDimensionHalfExtents();

    const Vec2 minDistance = box1HalfExtents + box2HalfExtents;

    const Vec2 delta = b2Centre - b1Centre;
    const Vec2 absDelta = Vec2::Abs(delta);

    if (absDelta.x > minDistance.x || absDelta.y > minDistance.y)
    {
        return;
    }

// Dev checks for handled and not handled collision.
#ifdef DEV_CONFIGURATION

    if (box1Collider->IsTrigger() || box2Collider->IsTrigger())
    {
        // Push both bodies mean both are dynamic.
        if (pushBothBodies && box2Collider->IsTrigger() || box1Collider->IsTrigger())
        {
            DEBUG_ERROR("Dynamic collision box is a trigger. This is not currently handled by the engine.");
            DEBUG_BREAK();
            return;
        }

        mTriggeredThisFrame[box2] = { .bodyCollider = box2Collider, .otherBody = box1};
        return;
    }

#endif // DEV_CONFIGURATION.

    if (box2Collider->IsTrigger())
    {
        mTriggeredThisFrame[box2] = { .bodyCollider = box2Collider, .otherBody = box1 };
        return;
    }

    if (const Vec2 overlap = minDistance - absDelta; overlap.x < overlap.y)
    {
        if (pushBothBodies)
        {
            const float halfOverlap = -0.5f * Math::Sign(delta.x) * overlap.x;
            box1Transform->translation.x += halfOverlap;
            box2Transform->translation.x -= halfOverlap;
            return;
        }

        box1Transform->translation.x -= Math::Sign(delta.x) * overlap.x;
    }
    else
    {
        if (pushBothBodies)
        {
            const float halfOverlap = -0.5f * Math::Sign(delta.y) * overlap.y;
            box1Transform->translation.y += halfOverlap;
            box2Transform->translation.y -= halfOverlap;
            return;
        }

        box1Transform->translation.y -= Math::Sign(delta.y) * overlap.y;
    }
}

} // Namespace ignite.