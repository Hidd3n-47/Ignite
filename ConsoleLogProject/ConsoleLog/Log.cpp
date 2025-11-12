#include "Log.h"

#using <DebugConsole.dll>
using namespace System;
using namespace DebugConsole;

namespace ignite
{

void Log::Init()
{
    ConsoleHost::Start();
}

void Log::Destroy()
{
    ConsoleHost::Stop();
}

} // Namespace ignite.