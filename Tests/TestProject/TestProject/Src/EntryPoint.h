#pragma once

#include "TestProject/Core/TestRegistry.h"

extern ignite::test::TestRegistry* ignite::test::CreateTestEnvironment();

inline int main(int, char**)
{
    ignite::test::TestRegistry* testRegistry = ignite::test::CreateTestEnvironment();

    testRegistry->Init();
    testRegistry->RunTests();

    delete testRegistry;
}
