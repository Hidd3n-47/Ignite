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
    mLogger.Debug("Total number of categories for test session \"{}\": {}", mTestSessionName, mTests.size());
    mLogger.Debug("| ==========================================================================================");

    uint32_t totalTestsRun = 0;
    for (const auto& [categoryName, tests] : mTests)
    {
        mLogger.Debug("|  -------------------- \"{}\" ----------------------", categoryName);

        uint32_t passed = 0;
        uint32_t failed = 0;

        for (size_t i = 0; i < tests.size(); ++i)
        {
            ++totalTestsRun;
            const Test& test = tests[i];

            if (const std::optional<std::string> result = test.Run(); !result.has_value())
            {
                mLogger.Info("| {} / {} | Test Passed: \"{}\"", i + 1, tests.size(), test.GetTestName());

                ++passed;
            }
            else
            {
                mLogger.Error("| {} / {} | Test FAILED: \"{}\"", i + 1, tests.size(), test.GetTestName());
                mLogger.Error("| \t\t{}", result.value());

                if (mOnTestFailedCallback)
                {
                    mOnTestFailedCallback();
                }

                ++failed;
            }
        }

        mLogger.Debug("| ----------------------------------------------", categoryName);
        mLogger.Debug("| \tTests run   : {}", tests.size());
        mLogger.Info ("| \tTests passed: {}", passed);
        if (failed > 0)
        {
            mLogger.Error("| \tTests failed: {}", failed);
        }
        else
        {
            mLogger.Info("| \tTests failed: {}", failed);
        }

        mPassedTests += passed;
        mFailedTests += failed;
    }

    mLogger.Debug("| ==========================================================================================\n");

    mLogger.Debug("> Tests run   : {}", totalTestsRun);
    mLogger.Info("> Tests passed: {}", mPassedTests);

    // Print the text in red if there were any failed tests.
    if(mFailedTests > 0)
    {
        mLogger.Error("> Tests failed: {}", mFailedTests);
    }
    else
    {
        mLogger.Info("> Tests failed: {}", mFailedTests);
    }
}

} // ignite::test.
