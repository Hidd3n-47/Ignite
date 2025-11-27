#pragma once

namespace ignite
{

class IRenderableComponent
{
public:
    virtual ~IRenderableComponent() = default;

    virtual void Render(mem::WeakRef<Renderer> renderer) { PROFILE_FUNC(); }
};

} // Namespace ignite.