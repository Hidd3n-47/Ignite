#pragma once

namespace ignite
{

struct CollisionInfo;

class RigidBody;
class GameObject;
class BoxCollider;

struct TriggeredColliderInfo
{
    mem::WeakHandle<BoxCollider> bodyCollider;
    mem::WeakHandle<GameObject>  otherBody;
};

class CollisionHandler
{
public:
    void Update();

    void AddDynamicBox(const mem::WeakHandle<GameObject> box);
    void AddStaticBox(const mem::WeakHandle<GameObject> box);

    void RemoveDynamicBox(const mem::WeakHandle<GameObject> box);
    void RemoveStaticBox(const mem::WeakHandle<GameObject> box);
private:
    std::vector<mem::WeakHandle<GameObject>> mDynamicBoxColliders;
    std::vector<mem::WeakHandle<GameObject>> mStaticBoxColliders;

    std::unordered_map<mem::WeakHandle<GameObject>, TriggeredColliderInfo> mTriggeredPrevFrame;
    std::unordered_map<mem::WeakHandle<GameObject>, TriggeredColliderInfo> mTriggeredThisFrame;

    void CheckCollisionBetweenBoxes(mem::WeakHandle<GameObject> box1, mem::WeakHandle<GameObject> box2, const bool pushBothBodies = false);
};

} // Namespace ignite.