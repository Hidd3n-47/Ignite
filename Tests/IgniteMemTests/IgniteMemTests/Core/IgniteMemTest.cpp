#include <TestProject/Src/EntryPoint.h>
#include <TestProject/Core/TestRegistry.h>

#include "Tests/MemoryManagerTests.h"

namespace ignite::test
{

class IgniteMemTest final : public TestRegistry
{
public:
    inline ~IgniteMemTest() override
    {
        mem::MemoryManager::Destroy();
    }

    inline void InitTestSession() override
    {
        mTestSessionName = "Ignite Mem Test";
    }

    inline void RegisterTests() override
    {
        MemoryManagerTests(this);
    }

};

TestRegistry* ignite::test::CreateTestEnvironment()
{
    return new IgniteMemTest();
}

} // Namespace ignite::test.
