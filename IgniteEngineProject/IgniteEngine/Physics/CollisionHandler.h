#pragma once

namespace ignite
{

struct CollisionInfo;

class RigidBody;
class GameObject;
class BoxCollider;

struct TriggeredColliderInfo
{
    mem::WeakRef<BoxCollider> bodyCollider;
    mem::WeakRef<GameObject>  otherBody;
};

class CollisionHandler
{
public:
    void Update();

    void AddDynamicBox(const mem::WeakRef<GameObject> box);
    void AddStaticBox(const mem::WeakRef<GameObject> box);

    void RemoveDynamicBox(const mem::WeakRef<GameObject> box);
    void RemoveStaticBox(const mem::WeakRef<GameObject> box);
private:
    std::vector<mem::WeakRef<GameObject>> mDynamicBoxColliders;
    std::vector<mem::WeakRef<GameObject>> mStaticBoxColliders;

    std::unordered_map<mem::WeakRef<GameObject>, TriggeredColliderInfo> mTriggeredPrevFrame;
    std::unordered_map<mem::WeakRef<GameObject>, TriggeredColliderInfo> mTriggeredThisFrame;

    void CheckCollisionBetweenBoxes(mem::WeakRef<GameObject> box1, mem::WeakRef<GameObject> box2, const bool pushBothBodies = false);
};

} // Namespace ignite.