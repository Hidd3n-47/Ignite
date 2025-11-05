#pragma once

#include <IgniteMem/Core/WeakRef.h>

#include "IgniteEngine/Core/OrthoCamera.h"

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
    virtual void Render(const OrthoCamera& camera) { }

protected:
    mem::WeakRef<GameObject> mParent;
};

} // Namespace ignite.