#pragma once

namespace ignite
{

class IUpdateableComponent
{
public:
    virtual ~IUpdateableComponent() = default;

    virtual void Update(const float dt) { PROFILE_FUNC(); }
};

} // Namespace ignite.