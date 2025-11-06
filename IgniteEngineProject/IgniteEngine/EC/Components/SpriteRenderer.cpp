#include "IgnitePch.h"
#include "SpriteRenderer.h"

#include "Core/Engine.h"
#include "EC/GameObject.h"
#include "Core/Rendering/TextureManager.h"

namespace ignite
{

SpriteRenderer::SpriteRenderer(const std::filesystem::path& filePath)
{
    mRenderCommand.texture = Engine::Instance()->GetTextureManager()->Load(filePath);
}

void SpriteRenderer::OnComponentAdded(const mem::WeakRef<GameObject> parent)
{
    IComponent::OnComponentAdded(parent);

    mRenderCommand.transform = mParent->GetComponent<Transform>();
}

void SpriteRenderer::Render(mem::WeakRef<Renderer> renderer)
{
    renderer->AddRenderCommand(0, mem::WeakRef{ &mRenderCommand });
}

} // Namespace ignite.