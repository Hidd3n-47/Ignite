#pragma once

#include "IgniteEngine/Src/IgnitePch.h"

#include "IgniteEngine/EC/IComponent.h"
#include "IgniteEngine/EC/IUpdateableComponent.h"
#include "IgniteEngine/EC/IRenderableComponent.h"

#include "IgniteEngine/EC/Components/Transform.h"

namespace ignite
{

class Scene;

class Renderer;
class IUpdateableComponent;
class IRenderableComponent;

/**
 * @class GameObject: A class representing the abstract idea of a 'Game Object'. A game object is a collection of \c Components.
 */
class GameObject
{
public:
    GameObject(const Ulid id, const mem::WeakRef<Scene> parent);
    ~GameObject();

    void Update(const float dt) const;
    void Render(mem::WeakRef<Renderer> renderer) const;

    [[nodiscard]] inline mem::WeakRef<Scene> GetScene() const { return mParent; }

    /**
     * @brief Add a component.
     * @tparam Component The component type being added.
     * @return A reference to the component that was added.
     */
    template <typename Component>
    mem::WeakRef<Component> AddComponent();

    /**
     * @brief Add A component and pass in the arguments into the component constructor.
     * @tparam Component The component type being added.
     * @tparam Args The argument types being forwarded into the components constructor.
     * @param args The arguments being forwarded into the components constructor.
     * @return
     */
    template <typename Component, typename... Args>
    mem::WeakRef<Component> AddComponent(Args ...args);

    /**
     * @brief Remove a component.
     * @tparam Component The component type that is being removed.
     */
    template <typename Component>
    void RemoveComponent();

    /**
     * @brief Get a component based of its type.
     * @tparam Component The type of the components that is being requested for.
     * @return A reference to the component. This reference might be invalid if not found
     * @see \c WeakRef.
     */
    template <typename Component>
    [[nodiscard]] mem::WeakRef<Component> GetComponent() const;

    /**
     * @brief Get the vector of components that are attached to the game object.
     * @return The vector of components attached to the game object.
     */
    [[nodiscard]] inline const std::vector<IComponent*>& GetComponents() const { return mComponents; }

    [[nodiscard]] inline Ulid GetId() const { return mId; }
private:
    Ulid mId;
    mem::WeakRef<Scene> mParent;

    std::vector<IComponent*> mComponents;
    std::vector<mem::WeakRef<IUpdateableComponent>> mUpdateableComponents;
    std::vector<mem::WeakRef<IRenderableComponent>> mRenderableComponents;
};

template <typename Component>
inline mem::WeakRef<Component> GameObject::AddComponent()
{
    mComponents.emplace_back(new Component());
    if (IUpdateableComponent* ref = dynamic_cast<IUpdateableComponent*>(mComponents.back()); ref)
    {
        mUpdateableComponents.emplace_back(ref);
    }
    if (IRenderableComponent* ref = dynamic_cast<IRenderableComponent*>(mComponents.back()); ref)
    {
        mRenderableComponents.emplace_back(ref);
    }
    mComponents.back()->OnComponentAdded(mem::WeakRef{ this });
    return mem::WeakRef{ reinterpret_cast<Component*>(mComponents.back()) };
}

template <typename Component, typename... Args>
inline mem::WeakRef<Component> GameObject::AddComponent(Args ...args)
{
    mComponents.emplace_back(new Component(std::forward<Args>(args)...));
    if (IUpdateableComponent* ref = dynamic_cast<IUpdateableComponent*>(mComponents.back()); ref)
    {
        mUpdateableComponents.emplace_back(ref);
    }
    if (IRenderableComponent* ref = dynamic_cast<IRenderableComponent*>(mComponents.back()); ref)
    {
        mRenderableComponents.emplace_back(ref);
    }
    mComponents.back()->OnComponentAdded(mem::WeakRef{ this });
    return mem::WeakRef{ reinterpret_cast<Component*>(mComponents.back()) };
}

template <typename Component>
void GameObject::RemoveComponent()
{
    for (auto it{ mComponents.begin() }; it != mComponents.end(); ++it)
    {
        if (Component* castComponent = dynamic_cast<Component*>(*it); castComponent != nullptr)
        {
            if (IUpdateableComponent* ref = dynamic_cast<IUpdateableComponent*>(mComponents.back()); ref)
            {
                std::erase(mUpdateableComponents, ref);
            }
            if (IRenderableComponent* ref = dynamic_cast<IRenderableComponent*>(mComponents.back()); ref)
            {
                std::erase(mUpdateableComponents, ref);
            }

            castComponent->OnComponentRemoved();

            mComponents.erase(it);

            delete castComponent;
            return;
        }
    }
}

template <typename Component>
inline mem::WeakRef<Component> GameObject::GetComponent() const
{
    for (IComponent* comp : mComponents)
    {
        if (Component* castComponent = dynamic_cast<Component*>(comp); castComponent != nullptr)
        {
            return mem::WeakRef{ castComponent };
        }
    }

    return mem::WeakRef<Component>{};
}

template <>
inline mem::WeakRef<Transform> GameObject::GetComponent() const
{
    return mem::WeakRef{ dynamic_cast<Transform*>(mComponents[0]) };
}

} // Namespace ignite.