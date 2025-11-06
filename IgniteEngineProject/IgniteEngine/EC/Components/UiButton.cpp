#include "IgnitePch.h"
#include "UiButton.h"

#include "Core/Engine.h"
#include "EC/GameObject.h"
#include "Core/Input/Keycode.h"
#include "Core/Input/InputManager.h"
#include "Core/Rendering/TextureManager.h"

namespace ignite
{

UiButton::UiButton(const std::filesystem::path& filePath, const bool hasPressedAnimation)
{
    mRenderCommand.texture         = Engine::Instance()->GetTextureManager()->Load(filePath);
    mRenderCommand.spritesheetMaxX = hasPressedAnimation ? 2 : 1;

    mInputManagerRef   = Engine::Instance()->GetInputManager();
}

void UiButton::OnComponentAdded(const mem::WeakRef<GameObject> parent)
{
    IComponent::OnComponentAdded(parent);

    mRenderCommand.transform = mParent->GetComponent<Transform>();
}

void UiButton::Update(const float dt)
{
    //todo verify as it seems like x-axis is slightly too much.
    //todo need to look at this when there is scaling as well.
    const Vec2 mousePosition = mInputManagerRef->GetMousePosition();
    const Vec2 positionScreenSpace = Engine::Instance()->GetCamera().PositionToScreenSpace(mParent->GetComponent<Transform>()->translation);

    const Vec2 delta = Vec2::Abs(mousePosition - positionScreenSpace);

    const bool wasHoveredPreviousFrame = mHovered;
    mHovered = delta.x <= mRenderCommand.texture.width * 0.5f && delta.y <= mRenderCommand.texture.height * 0.5f;

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

void UiButton::Render(mem::WeakRef<Renderer> renderer)
{
    mRenderCommand.spritesheetPosX = std::min(static_cast<float>(mMouseDown),mRenderCommand.spritesheetMaxX - 1);
    renderer->AddRenderCommand(0, mem::WeakRef{ &mRenderCommand });
}

} // Namespace ignite.