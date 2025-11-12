#pragma once

namespace ignite
{

class GameObject;

struct CollisionInfo
{
    mem::WeakRef<GameObject> instance;
    mem::WeakRef<GameObject> other;
    Vec2 collisionPosition;
};

} // Namespace ignite.