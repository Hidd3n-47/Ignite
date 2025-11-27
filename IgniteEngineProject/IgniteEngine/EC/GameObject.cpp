#include "IgnitePch.h"
#include "GameObject.h"

#include "EC/Scene.h"

#include "Components/Transform.h"

namespace ignite
{

GameObject::GameObject(const Ulid id, const mem::WeakHandle<Scene> parent)
    : mId(id)
    , mParent(parent)
{
    AddComponent<Transform>();
}

GameObject::~GameObject()
{
    PROFILE_FUNC();

    for (IComponent* comp : mComponents)
    {
        comp->OnComponentRemoved();
        delete comp;
    }
}

void GameObject::Update(const float dt) const
{
    PROFILE_FUNC();

    for (mem::WeakHandle<IUpdateableComponent> component : mUpdateableComponents)
    {
        component->Update(dt);
    }
}

void GameObject::Render(mem::WeakHandle<Renderer> renderer) const
{
    PROFILE_FUNC();

    for (mem::WeakHandle<IRenderableComponent> component : mRenderableComponents)
    {
        component->Render(renderer);
    }
}

} // Namespace ignite.
