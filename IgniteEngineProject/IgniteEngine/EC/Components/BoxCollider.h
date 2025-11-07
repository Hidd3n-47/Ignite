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

    void OnTriggerEnter(const mem::WeakRef<GameObject> other) const { if (mOnTriggeredCallback) mOnTriggeredCallback(other); }

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

    void SetOnTriggeredCallback(const std::function<void(mem::WeakRef<GameObject>)>& onTriggeredCallback) { mOnTriggeredCallback = onTriggeredCallback; }
    void SetOffset(const Vec2 offset);
private:
    Vec2 mDimensionHalfExtents;
    Vec2 mOffset;
    bool mDynamic;
    bool mTrigger;

    std::function<void(mem::WeakRef<GameObject>)> mOnTriggeredCallback;
};

} // Namespace ignite.