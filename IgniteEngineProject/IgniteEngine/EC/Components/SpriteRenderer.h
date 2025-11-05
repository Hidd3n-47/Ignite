#pragma once

#include "IgniteEngine/EC/IComponent.h"

#include "IgniteEngine/Core/Rendering/Texture.h"

namespace ignite
{

class InputManager;
class TextureManager;

class SpriteRenderer : public IComponent
{
public:
    SpriteRenderer(const std::filesystem::path& filePath);

    void Render(const OrthoCamera& camera) override;

    [[nodiscard]] inline Texture GetTexture() const { return mTexture; }
private:
    Texture mTexture;

    mem::WeakRef<TextureManager> mTextureManagerRef;
};

} // Namespace ignite.