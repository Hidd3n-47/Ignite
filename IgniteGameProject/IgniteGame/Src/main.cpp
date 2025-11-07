#include <IgniteEngine/Core/Engine.h>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>


#include "Core/GameManager.h"

int main(int, char**)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    ignite::Engine::CreateEngine()->Init();
    ignite::GameManager::CreateGameManager()->Init();

    ignite::Engine::Instance()->Run();

    ignite::GameManager::Instance()->Destroy();
    ignite::Engine::Instance()->Destroy();

    _CrtDumpMemoryLeaks();
}
