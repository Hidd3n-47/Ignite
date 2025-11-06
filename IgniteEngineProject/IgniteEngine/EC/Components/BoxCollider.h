#pragma once

#include "IgniteEngine/EC/IComponent.h"

#include "IgniteEngine/Core/Rendering/RenderCommand.h"

namespace ignite
{

class InputManager;
class TextureManager;

class BoxCollider : public IComponent
{
public:
    BoxCollider(const Vec2 dimensionHalfExtents, const bool dynamic = false, const bool trigger = false);

    void OnComponentAdded(const mem::WeakRef<GameObject> parent) override;
    void OnComponentRemoved() override;

    void Update(const float dt) override;

#ifdef DEV_CONFIGURATION
public:
    void Render(mem::WeakRef<Renderer> renderer) override;
private:
    RenderCommand mRenderCommand{ };
public:
#endif // DEV_CONFIGURATION.

    [[nodiscard]] inline bool IsTrigger() const { return mTrigger; }
    [[nodiscard]] inline Vec2 GetOffset() const { return mOffset; }
    [[nodiscard]] inline Vec2 GetDimensionHalfExtents() const { return mDimensionHalfExtents; }

    inline void SetOffset(const Vec2 offset)
    {
        mOffset = offset;
#ifdef DEV_CONFIGURATION
        mRenderCommand.debugSquareOffset = offset;
#endif // DEV_CONFIGURATION.
    }
private:
    Vec2 mDimensionHalfExtents;
    Vec2 mOffset;
    bool mDynamic;
    bool mTrigger;
};

} // Namespace ignite.