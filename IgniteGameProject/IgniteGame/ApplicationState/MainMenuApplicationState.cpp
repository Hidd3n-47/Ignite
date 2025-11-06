#include "MainMenuApplicationState.h"

#include <IgniteMem/Core/WeakRef.h>

#include <IgniteEngine/EC/Scene.h>
#include <IgniteEngine/Core/Engine.h>
#include <IgniteEngine/EC/Components/UiButton.h>
#include <IgniteEngine/EC/Components/Transform.h>

#include "Core/GameManager.h"

namespace ignite
{
void MainMenuApplicationState::InitScene()
{

    mem::WeakRef<GameObject> playButtonObject = CreateGameObject();
    playButtonObject->GetComponent<Transform>()->translation = Vec2{ 0.0f, 2.0f };
    mem::WeakRef<UiButton> playButton = playButtonObject->AddComponent<UiButton>(std::filesystem::path{ "E:/Programming/Ignite/Assets/PlayButton.png" });
    playButton->SetOnButtonPressedEvent([] { GameManager::Instance()->ChangeState(ApplicationStates::GAME); });
    playButton->SetOnHoveredEvent(
        [](mem::WeakRef<GameObject> gameObject)
        {
            gameObject->GetComponent<Transform>()->scale = Vec2{ 1.2f };
        });
    playButton->SetOnRestingStateEvent(
        [](mem::WeakRef<GameObject> gameObject)
        {
            gameObject->GetComponent<Transform>()->scale = Vec2{ 1.0f };
        });

    mem::WeakRef<GameObject> exitButtonObject = CreateGameObject();
    exitButtonObject->GetComponent<Transform>()->translation = Vec2{ 0.0f, -2.0f };
    exitButtonObject->AddComponent<UiButton>(std::filesystem::path{ "E:/Programming/Ignite/Assets/QuitButton.png" });
}

void MainMenuApplicationState::SceneUpdate() const
{
    Scene::SceneUpdate();
}
} // Namespace ignite.