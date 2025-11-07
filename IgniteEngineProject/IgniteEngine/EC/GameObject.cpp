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
    for (IComponent* comp : mComponents)
    {
        comp->OnComponentRemoved();
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

void GameObject::Render(mem::WeakRef<Renderer> renderer) const
{
    for (IComponent* component : mComponents)
    {
        component->Render(renderer);
    }
}

} // Namespace ignite.
