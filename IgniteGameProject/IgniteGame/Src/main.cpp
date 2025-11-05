#include <IgniteEngine/Core/Engine.h>

#include "Core/GameManager.h"

int main(int, char**)
{
    ignite::Engine::CreateEngine()->Init();
    ignite::GameManager::CreateGameManager()->Init();

    ignite::Engine::Instance()->Run();

    ignite::GameManager::Instance()->Destroy();
    ignite::Engine::Instance()->Destroy();
}
