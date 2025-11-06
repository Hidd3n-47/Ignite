#pragma once

#include <IgniteMem/Core/WeakRef.h>

#include "IgniteEngine/Core/Rendering/Renderer.h"

namespace ignite
{

class GameObject;

/**
 * @class IComponent: A class that gives components its bare functionality.
 * @note All components should inherit from this.
 */
class IComponent
{
public:
    virtual ~IComponent() = default;

    virtual void OnComponentAdded(const mem::WeakRef<GameObject> parent) { mParent = parent; }
    virtual void OnComponentRemoved() { }

    virtual void Update(const float dt) { }
    virtual void Render(mem::WeakRef<Renderer> renderer) { }

protected:
    mem::WeakRef<GameObject> mParent;
};

} // Namespace ignite.