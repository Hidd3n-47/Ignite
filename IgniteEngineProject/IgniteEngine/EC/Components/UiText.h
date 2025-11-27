#pragma once

#include "IgniteEngine/EC/IComponent.h"

namespace ignite
{

class FontRenderer;

class UiText : public IComponent
{
public:
    UiText(std::string text, const float size);

    void OnComponentAdded(const mem::WeakHandle<GameObject> parent) override;
    void OnComponentRemoved() override;

    void SetText(const std::string& text);
private:
    mem::WeakHandle<FontRenderer> mFontRendererRef;

    uint16_t    mId{};
    std::string mText;
    float       mSize;
};

} // Namespace ignite.