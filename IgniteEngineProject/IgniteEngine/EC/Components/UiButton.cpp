#include "IgnitePch.h"
#include "UiButton.h"

#include "Core/Engine.h"
#include "EC/GameObject.h"
#include "Core/Input/Keycode.h"
#include "Core/Input/InputManager.h"
#include "Core/Rendering/TextureManager.h"

namespace ignite
{

UiButton::UiButton(const std::filesystem::path& filePath)
{
    mTexture = Engine::Instance()->GetTextureManager()->Load(filePath);

    mInputManagerRef   = Engine::Instance()->GetInputManager();
    mTextureManagerRef = Engine::Instance()->GetTextureManager();
}

void UiButton::Update(const float dt)
{
    //todo verify as it seems like x-axis is slightly too much.
    //todo need to look at this when there is scaling as well.
    const Vec2 mousePosition = mInputManagerRef->GetMousePosition();
    const Vec2 positionScreenSpace = Engine::Instance()->GetCamera().PositionToScreenSpace(mParent->GetComponent<Transform>()->translation);

    const Vec2 delta = Vec2::Abs(mousePosition - positionScreenSpace);

    const bool wasHoveredPreviousFrame = mHovered;
    mHovered = delta.x <= mTexture.width * 0.5f && delta.y <= mTexture.height * 0.5f;

    // If the button isn't hovered and was hovered, call resting state callback and early exit.
    if (!mHovered)
    {
        if (wasHoveredPreviousFrame && mOnRestingStateEvent)
        {
            mOnRestingStateEvent(mParent);
        }

        mMouseDown = false;
        return;
    }

    if (mOnHoveredEvent)
    {
        mOnHoveredEvent(mParent);
    }

    if (mInputManagerRef->IsMousePressed(Mouse::BUTTON_LEFT))
    {
        mMouseDown = true;
    }
    // If the mouse isn't down, but was down, we have released, therefore do the on pressed callback if it exists.
    else if (!mInputManagerRef->IsMouseDown(Mouse::BUTTON_LEFT) && mMouseDown && mOnButtonPressedEvent)
    {
        mOnButtonPressedEvent();
    }
}

void UiButton::Render(const OrthoCamera& camera)
{
    mTextureManagerRef->RenderSingleFromSpriteSheet(mTexture, mParent->GetComponent<Transform>(), camera, mMouseDown, 0, 2, 1);
}

} // Namespace ignite.