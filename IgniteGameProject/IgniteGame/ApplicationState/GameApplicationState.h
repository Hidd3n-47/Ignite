#pragma once

#include <IgniteEngine/EC/Scene.h>

namespace ignite
{

class InputManager;

class GameApplicationState : public Scene
{
public:
    void InitScene() override;
    void SceneUpdate() const override;
private:
    mem::WeakRef<GameObject> mPlayer;

    mem::WeakRef<InputManager> mInputManager;
};

} // Namespace ignite.