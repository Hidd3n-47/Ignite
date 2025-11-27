#pragma once

#include <IgniteMem/Core/WeakHandle.h>

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

    virtual void OnComponentAdded(const mem::WeakHandle<GameObject> parent) { mParent = parent; }
    virtual void OnComponentRemoved() { PROFILE_FUNC(); }

    [[nodiscard]] inline mem::WeakHandle<GameObject> GetParent() const { return mParent; }
protected:
    mem::WeakHandle<GameObject> mParent;
};

} // Namespace ignite.