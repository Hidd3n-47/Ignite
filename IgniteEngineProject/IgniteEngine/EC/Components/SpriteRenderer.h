#pragma once

#include "IgniteEngine/EC/IComponent.h"

#include "IgniteEngine/Core/Rendering/RenderCommand.h"

namespace ignite
{

class InputManager;

class SpriteRenderer : public IComponent
{
public:
    SpriteRenderer(const std::filesystem::path& filePath, const uint32_t layer = 0);
    SpriteRenderer(const Texture& texture, const float xSpritesheet = 0.0f, const float ySpritesheet = 0.0f, const uint32_t layer = 0);

    void OnComponentAdded(const mem::WeakRef<GameObject> parent) override;
    void Render(mem::WeakRef<Renderer> renderer) override;
private:
    uint32_t mLayer;
    RenderCommand mRenderCommand{ };
};

} // Namespace ignite.