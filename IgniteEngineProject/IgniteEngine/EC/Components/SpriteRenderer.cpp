#include "IgnitePch.h"
#include "SpriteRenderer.h"

#include "Core/Engine.h"
#include "EC/GameObject.h"
#include "Core/Rendering/TextureManager.h"

namespace ignite
{

SpriteRenderer::SpriteRenderer(const std::filesystem::path& filePath)
{
    mTexture = Engine::Instance()->GetTextureManager()->Load(filePath);

    mTextureManagerRef = Engine::Instance()->GetTextureManager();
}

void SpriteRenderer::Render(const OrthoCamera& camera)
{
    mTextureManagerRef->RenderSingle(mTexture, mParent->GetComponent<Transform>(), camera);
}

} // Namespace ignite.