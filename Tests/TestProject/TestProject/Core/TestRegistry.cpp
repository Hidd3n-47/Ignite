#include "TestRegistry.h"

#include <Logger/Log.h>

namespace ignite::test
{

void TestRegistry::Init()
{
    InitTestSession();

    RegisterTests();
}

void TestRegistry::RunTests()
{
    Log::Debug("Tests", "Total number of categories for test session \"{}\": {}", mTestSessionName, mTests.size());
    Log::Debug("Tests", "| ==========================================================================================");

    uint32_t totalTestsRun = 0;
    for (const auto& [categoryName, tests] : mTests)
    {
        Log::Debug("Tests", "|  -------------------- \"{}\" ----------------------", categoryName);

        uint32_t passed = 0;
        uint32_t failed = 0;

        for (size_t i = 0; i < tests.size(); ++i)
        {
            ++totalTestsRun;
            const Test& test = tests[i];

            if (const std::optional<std::string> result = test.Run(); !result.has_value())
            {
                Log::Info("Tests", "| {} / {} | Test Passed: \"{}\"", i + 1, tests.size(), test.GetTestName());

                ++passed;
            }
            else
            {
                Log::Error("Tests", "| {} / {} | Test FAILED: \"{}\"", i + 1, tests.size(), test.GetTestName());
                Log::Error("Tests", "| \t\t{}", result.value());

                if (mOnTestFailedCallback)
                {
                    mOnTestFailedCallback();
                }

                ++failed;
            }
        }

        Log::Debug("Tests", "| ----------------------------------------------", categoryName);
        Log::Debug("Tests", "| \tTests run   : {}", tests.size());
        Log::Info ("Tests", "| \tTests passed: {}", passed);
        if (failed > 0)
        {
            Log::Error("Tests", "| \tTests failed: {}", failed);
        }
        else
        {
            Log::Info("Tests", "| \tTests failed: {}", failed);
        }

        mPassedTests += passed;
        mFailedTests += failed;
    }

    Log::Debug("Tests", "| ==========================================================================================\n");

    Log::Debug("Tests", "> Tests run   : {}", totalTestsRun);
    Log::Info("Tests", "> Tests passed: {}", mPassedTests);

    // Print the text in red if there were any failed tests.
    if(mFailedTests > 0)
    {
        Log::Error("Tests", "> Tests failed: {}", mFailedTests);
    }
    else
    {
        Log::Info("Tests", "> Tests failed: {}", mFailedTests);
    }
}

} // ignite::test.
