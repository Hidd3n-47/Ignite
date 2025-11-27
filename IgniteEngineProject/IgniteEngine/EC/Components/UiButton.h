#pragma once

#include "IgniteEngine/EC/IComponent.h"
#include "IgniteEngine/EC/IUpdateableComponent.h"
#include "IgniteEngine/EC/IRenderableComponent.h"

#include "IgniteEngine/Core/Rendering/RenderCommand.h"

namespace ignite
{

class InputManager;
class TextureManager;

class UiButton : public IComponent, public IUpdateableComponent, public IRenderableComponent
{
public:
    UiButton(const char* filePath, const bool hasPressedAnimation = false);

    void OnComponentAdded(const mem::WeakHandle<GameObject> parent) override;

    void Update(const float dt) override;
    void Render(mem::WeakHandle<Renderer> renderer) override;

    inline void SetOnHoveredEvent(const std::function<void(mem::WeakHandle<GameObject>)>& onHoveredCallback)           { mOnHoveredEvent       = onHoveredCallback; }
    inline void SetOnRestingStateEvent(const std::function<void(mem::WeakHandle<GameObject>)>& onRestingStateCallback) { mOnRestingStateEvent  = onRestingStateCallback; }
    inline void SetOnButtonPressedEvent(const std::function<void()>& onButtonPressedCallback)                       { mOnButtonPressedEvent = onButtonPressedCallback; }

private:
    RenderCommand mRenderCommand{ };

    std::function<void(mem::WeakHandle<GameObject>)> mOnHoveredEvent;
    std::function<void(mem::WeakHandle<GameObject>)> mOnRestingStateEvent;
    std::function<void()> mOnButtonPressedEvent;

    bool mHovered  { false };
    bool mMouseDown{ false };

    mem::WeakHandle<InputManager>   mInputManagerRef;
};

} // Namespace ignite.