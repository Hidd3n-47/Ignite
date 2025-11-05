#include "IgnitePch.h"
#include "UiButton.h"

#include "Core/Engine.h"
#include "EC/GameObject.h"
#include "Core/Input/Keycode.h"
#include "Core/Input/InputManager.h"
#include "Core/Rendering/TextureManager.h"

namespace ignite
{

UiButton::UiButton(const std::filesystem::path& filePath, const std::function<void()>& onClickedEvent /*= nullptr*/)
    : mOnClickedEvent(onClickedEvent)
{
    mTexture = Engine::Instance()->GetTextureManager()->Load(filePath);

    mInputManagerRef   = Engine::Instance()->GetInputManager();
    mTextureManagerRef = Engine::Instance()->GetTextureManager();
}

UiButton::UiButton(const Texture texture)
    : mTexture(texture)
{
    mInputManagerRef   = Engine::Instance()->GetInputManager();
    mTextureManagerRef = Engine::Instance()->GetTextureManager();
}

void UiButton::Update(const float dt)
{
    //todo verify as it seems like x-axis is slightly too much.
    const Vec2 mousePosition = mInputManagerRef->GetMousePosition();
    const Vec2 positionScreenSpace = Engine::Instance()->GetCamera().PositionToScreenSpace(mParent->GetComponent<Transform>()->translation);

    const Vec2 delta = Vec2::Abs(mousePosition - positionScreenSpace);

    mHovered = delta.x <= mTexture.width * 0.5f && delta.y <= mTexture.height * 0.5f;

    if (mHovered && mInputManagerRef->IsMouseDown(Mouse::BUTTON_LEFT) && mOnClickedEvent)
    {
        mOnClickedEvent();
    }
}

void UiButton::Render(const OrthoCamera& camera)
{
    mTextureManagerRef->RenderSingleFromSpriteSheet(mTexture, mParent->GetComponent<Transform>(), camera, mHovered, 0, 2, 1);
}

} // Namespace ignite.