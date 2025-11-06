#pragma once

#include <IgniteEngine/EC/Scene.h>

namespace ignite
{

class LevelSelectApplicationState : public Scene
{
public:
    void InitScene() override;
    void SceneUpdate() const override;
};

} // Namespace ignite.