#include <IgniteEngine/Core/Engine.h>

int main(int, char**)
{
    ignite::Engine::CreateEngine()->Init();
    ignite::Engine::Instance()->Run();
    ignite::Engine::Instance()->Destroy();
}