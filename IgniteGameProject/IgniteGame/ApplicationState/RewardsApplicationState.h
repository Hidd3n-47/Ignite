#pragma once

#include <IgniteEngine/EC/Scene.h>

namespace ignite
{

class RewardsApplicationState : public Scene
{
public:
    RewardsApplicationState() = default;

    void InitScene() override;
    void SceneUpdate() const override;
};

} // Namespace ignite.