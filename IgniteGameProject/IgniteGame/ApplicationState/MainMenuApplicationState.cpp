#include "MainMenuApplicationState.h"

#include <IgniteMem/Core/WeakRef.h>

#include <IgniteEngine/EC/Scene.h>
#include <IgniteEngine/EC/Components/UiButton.h>
#include <IgniteEngine/EC/Components/Transform.h>

namespace ignite
{
void MainMenuApplicationState::InitScene()
{

    mem::WeakRef<GameObject> playButtonObject = CreateGameObject();
    playButtonObject->GetComponent<Transform>()->translation = Vec2{ 0.0f, 2.0f };
    auto playButton = playButtonObject->AddComponent<UiButton>(std::filesystem::path{ "E:/Programming/Ignite/Assets/PlayButton.png" });

    mem::WeakRef<GameObject> exitButtonObject = CreateGameObject();
    exitButtonObject->GetComponent<Transform>()->translation = Vec2{ 0.0f, -2.0f };
    exitButtonObject->AddComponent<UiButton>(playButton->GetTexture());
}

void MainMenuApplicationState::SceneUpdate() const
{
    Scene::SceneUpdate();
}
} // Namespace ignite.