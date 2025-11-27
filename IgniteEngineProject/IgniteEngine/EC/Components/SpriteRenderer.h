#pragma once

#include "IgniteEngine/EC/IComponent.h"
#include "IgniteEngine/EC/IRenderableComponent.h"

#include "IgniteEngine/Core/Rendering/RenderCommand.h"

namespace ignite
{

class InputManager;

class SpriteRenderer : public IComponent, public IRenderableComponent
{
public:
    SpriteRenderer(const char* filePath, const uint32_t layer = 0);
    SpriteRenderer(const Texture& texture, const float xSpritesheet = 0.0f, const float ySpritesheet = 0.0f, const uint32_t layer = 0);

    void OnComponentAdded(const mem::WeakHandle<GameObject> parent) override;
    void Render(mem::WeakHandle<Renderer> renderer) override;
private:
    uint32_t mLayer;
    RenderCommand mRenderCommand{ };
};

} // Namespace ignite.