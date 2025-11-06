#pragma once

#include "IgniteEngine/EC/IComponent.h"

#include "IgniteEngine/Core/Rendering/RenderCommand.h"

namespace ignite
{

class InputManager;
class TextureManager;

class SpriteRenderer : public IComponent
{
public:
    SpriteRenderer(const std::filesystem::path& filePath);

    void OnComponentAdded(const mem::WeakRef<GameObject> parent) override;
    void Render(mem::WeakRef<Renderer> renderer) override;
private:
    RenderCommand mRenderCommand{ };
};

} // Namespace ignite.