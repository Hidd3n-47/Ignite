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
    UiButton(const std::filesystem::path& filePath, const uint32_t spritesheetIndexNonHover = 0, const uint32_t spritesheetIndexOnHover = 0);
    UiButton(const Texture texture, const uint32_t spritesheetIndexNonHover = 0, const uint32_t spritesheetIndexOnHover = 0);

    void Update(const float dt) override;
    void Render(const OrthoCamera& camera) override;

    [[nodiscard]] inline Texture GetTexture() const { return mTexture; }
private:
    Texture mTexture;

    uint32_t mSpritesheetIndexNonHover{ 0 };
    uint32_t mSpritesheetIndexOnHover{ 0 };
    bool mHovered{ false };

    mem::WeakRef<InputManager>   mInputManagerRef;
    mem::WeakRef<TextureManager> mTextureManagerRef;
};

} // Namespace ignite.