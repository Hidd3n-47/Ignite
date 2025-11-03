#pragma once

#include "IgniteEngine/EC/IComponent.h"

namespace ignite
{

class Transform : public IComponent
{
public:
    Vec2  translation = Vec2{ 0.0f, 0.0f };
    float rotation    = 0.0f;
};

} // Namespace ignite.