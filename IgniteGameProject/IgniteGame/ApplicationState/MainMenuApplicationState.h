#pragma once

#include <IgniteEngine/EC/Scene.h>

namespace ignite
{

class MainMenuApplicationState : public Scene
{
public:
    void InitScene() override;
    void SceneUpdate() const override;
};

} // Namespace ignite.