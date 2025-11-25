#include "IgnitePch.h"
#include "UiText.h"

#include "Core/Engine.h"
#include "EC/GameObject.h"
#include "Core/Rendering/FontRenderer.h"

namespace ignite
{

UiText::UiText(std::string text, const float size)
    : mText(std::move(text))
    , mSize(size)
{
    PROFILE_FUNC();

    mFontRendererRef = Engine::Instance()->GetFontRenderer();
}

void UiText::OnComponentAdded(const mem::WeakRef<GameObject> parent)
{
    PROFILE_FUNC();

    IComponent::OnComponentAdded(parent);

    mId = mFontRendererRef->CreateFont("Assets/Fonts/ThaleahFat.ttf", mSize, mText, mParent->GetComponent<Transform>());
}

void UiText::OnComponentRemoved()
{
    PROFILE_FUNC();

    mFontRendererRef->RemoveFont(mId);
}

void UiText::SetText(const std::string& text)
{
    PROFILE_FUNC();

    mFontRendererRef->UpdateFont(mId, text);

    mText = text;
}

} // Namespace ignite.