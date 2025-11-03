#include "IgnitePch.h"
#include "GameObject.h"

#include "EC/Scene.h"

#include "Components/Transform.h"

namespace ignite
{

GameObject::GameObject(const Ulid id, const mem::WeakRef<Scene> parent)
    : mId(id)
    , mParent(parent)
{
    AddComponent<Transform>();
}

GameObject::~GameObject()
{
    for (const auto comp : mComponents)
    {
        delete comp;
    }
}

void GameObject::Update(const float dt) const
{
    for (IComponent* component : mComponents)
    {
        component->Update(dt);
    }
}

void GameObject::Render() const
{
    for (IComponent* component : mComponents)
    {
        component->Render();
    }
}

} // Namespace ignite.
