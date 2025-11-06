#include "IgnitePch.h"
#include "SpriteRenderer.h"

#include "Core/Engine.h"
#include "EC/GameObject.h"
#include "Core/Rendering/TextureManager.h"

namespace ignite
{

SpriteRenderer::SpriteRenderer(const std::filesystem::path& filePath, const uint32_t layer)
    : mLayer(layer)
{
    Engine::Instance()->GetTextureManager()->Load(mRenderCommand.texture, filePath);
}

SpriteRenderer::SpriteRenderer(const Texture& texture, const float xSpritesheet, const float ySpritesheet, const uint32_t layer)
{
    mRenderCommand.texture         = texture;
    mRenderCommand.spritesheetPosX = xSpritesheet;
    mRenderCommand.spritesheetPosY = ySpritesheet;

    mLayer = layer;
}

void SpriteRenderer::OnComponentAdded(const mem::WeakRef<GameObject> parent)
{
    IComponent::OnComponentAdded(parent);

    mRenderCommand.transform = mParent->GetComponent<Transform>();
}

void SpriteRenderer::Render(mem::WeakRef<Renderer> renderer)
{
    renderer->AddRenderCommand(mLayer, mem::WeakRef{ &mRenderCommand });
}

} // Namespace ignite.