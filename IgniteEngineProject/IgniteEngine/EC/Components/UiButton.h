#pragma once

#include "IgniteEngine/EC/IComponent.h"

#include "IgniteEngine/Core/Rendering/Texture.h"

namespace ignite
{

class InputManager;
class TextureManager;

class UiButton : public IComponent
{
public:
    UiButton(const std::filesystem::path& filePath);

    void Update(const float dt) override;
    void Render(const OrthoCamera& camera) override;

    inline void SetOnHoveredEvent(const std::function<void(mem::WeakRef<GameObject>)>& onHoveredCallback)           { mOnHoveredEvent       = onHoveredCallback; }
    inline void SetOnRestingStateEvent(const std::function<void(mem::WeakRef<GameObject>)>& onRestingStateCallback) { mOnRestingStateEvent  = onRestingStateCallback; }
    inline void SetOnButtonPressedEvent(const std::function<void()>& onButtonPressedCallback)                       { mOnButtonPressedEvent = onButtonPressedCallback; }

    [[nodiscard]] inline Texture GetTexture() const { return mTexture; }
    int index = 0;
private:
    Texture mTexture;

    std::function<void(mem::WeakRef<GameObject>)> mOnHoveredEvent;
    std::function<void(mem::WeakRef<GameObject>)> mOnRestingStateEvent;
    std::function<void()> mOnButtonPressedEvent;

    bool mHovered  { false };
    bool mMouseDown{ false };

    mem::WeakRef<InputManager>   mInputManagerRef;
    mem::WeakRef<TextureManager> mTextureManagerRef;
};

} // Namespace ignite.