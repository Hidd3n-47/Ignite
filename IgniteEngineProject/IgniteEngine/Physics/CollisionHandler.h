#pragma once

namespace ignite
{

struct CollisionInfo;

class RigidBody;
class GameObject;
class CircleCollider;

class CollisionHandler
{
public:
    void Update() const;

    void AddDynamicBox(const mem::WeakRef<GameObject> box);
    void AddStaticBox(const mem::WeakRef<GameObject> box);

    void RemoveDynamicBox(const mem::WeakRef<GameObject> box);
    void RemoveStaticBox(const mem::WeakRef<GameObject> box);
private:
    std::vector<mem::WeakRef<GameObject>> mDynamicBoxColliders;
    std::vector<mem::WeakRef<GameObject>> mStaticBoxColliders;

    static void CheckCollisionBetweenBoxes(mem::WeakRef<GameObject> box1, mem::WeakRef<GameObject> box2, const bool pushBothBodies = false);
};

} // Namespace ignite.