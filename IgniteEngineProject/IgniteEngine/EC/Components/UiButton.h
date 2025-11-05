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
    UiButton(const std::filesystem::path& filePath, const std::function<void()>& onClickedEvent = nullptr);
    UiButton(const Texture texture);

    void Update(const float dt) override;
    void Render(const OrthoCamera& camera) override;

    [[nodiscard]] inline Texture GetTexture() const { return mTexture; }
private:
    Texture mTexture;
    std::function<void()> mOnClickedEvent;

    bool mHovered{ false };

    mem::WeakRef<InputManager>   mInputManagerRef;
    mem::WeakRef<TextureManager> mTextureManagerRef;
};

} // Namespace ignite.