#include "IgnitePch.h"
//#include "PhysicsManager.h"
//
//#include "ES/GameObject.h"
//#include "ES/Components/RigidBody.h"
//#include "Es/Components/Transform.h"
//
//namespace ignite
//{
//
//void PhysicsManager::Update(const float dt) const
//{
//    for (const WeakRef<GameObject> body : mBodies)
//    {
//        WeakRef<RigidBody> rigidBody = body->GetComponent<RigidBody>();
//        if (!rigidBody->IsEnabled())
//        {
//            continue;
//        }
//
//        WeakRef<Transform> transform = body->GetComponent<Transform>();
//
//        const Vec2 acceleration = (rigidBody->mForce + rigidBody->mGravity) / rigidBody->mMass;
//
//        rigidBody->SetVelocity(rigidBody->mVelocity + acceleration * dt);
//
//        transform->translation += rigidBody->mVelocity * dt;
//
//        rigidBody->mForce = Vec2{ 0.0f, 0.0f };
//    }
//}
//
//void PhysicsManager::AddBody(const WeakRef<GameObject> body)
//{
//    mBodies.emplace_back(body);
//}
//
//void PhysicsManager::RemoveBody(const WeakRef<GameObject> body)
//{
//    for (auto b = mBodies.begin(); b != mBodies.end(); ++b)
//    {
//        if (*b == body)
//        {
//            mBodies.erase(b);
//            return;
//        }
//    }
//}
//
//} // Namespace ignite.