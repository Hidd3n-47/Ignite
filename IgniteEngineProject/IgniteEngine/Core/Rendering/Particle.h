#pragma once

#include "IgniteEngine/EC/Components/Transform.h"
#include "IgniteEngine/Core/Rendering/RenderCommand.h"

namespace ignite
{

struct Particle
{
    inline Particle()
    {
        renderCommand.transform = mem::WeakHandle{ &transform };
    }

    float         lifetime{};
    float         initialLifetime{};
    Transform     transform{};
    RenderCommand renderCommand{};
};

} // Namespace ignite.