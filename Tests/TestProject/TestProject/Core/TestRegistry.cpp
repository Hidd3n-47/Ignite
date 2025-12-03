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
    Log logger{ "Tests" };
    logger.Debug("Tests", "Total number of categories for test session \"{}\": {}", mTestSessionName, mTests.size());
    logger.Debug("Tests", "| ==========================================================================================");

    uint32_t totalTestsRun = 0;
    for (const auto& [categoryName, tests] : mTests)
    {
        logger.Debug("Tests", "|  -------------------- \"{}\" ----------------------", categoryName);

        uint32_t passed = 0;
        uint32_t failed = 0;

        for (size_t i = 0; i < tests.size(); ++i)
        {
            ++totalTestsRun;
            const Test& test = tests[i];

            if (const std::optional<std::string> result = test.Run(); !result.has_value())
            {
                logger.Info("Tests", "| {} / {} | Test Passed: \"{}\"", i + 1, tests.size(), test.GetTestName());

                ++passed;
            }
            else
            {
                logger.Error("Tests", "| {} / {} | Test FAILED: \"{}\"", i + 1, tests.size(), test.GetTestName());
                logger.Error("Tests", "| \t\t{}", result.value());

                if (mOnTestFailedCallback)
                {
                    mOnTestFailedCallback();
                }

                ++failed;
            }
        }

        logger.Debug("Tests", "| ----------------------------------------------", categoryName);
        logger.Debug("Tests", "| \tTests run   : {}", tests.size());
        logger.Info ("Tests", "| \tTests passed: {}", passed);
        if (failed > 0)
        {
            logger.Error("Tests", "| \tTests failed: {}", failed);
        }
        else
        {
            logger.Info("Tests", "| \tTests failed: {}", failed);
        }

        mPassedTests += passed;
        mFailedTests += failed;
    }

    logger.Debug("Tests", "| ==========================================================================================\n");

    logger.Debug("Tests", "> Tests run   : {}", totalTestsRun);
    logger.Info("Tests", "> Tests passed: {}", mPassedTests);

    // Print the text in red if there were any failed tests.
    if(mFailedTests > 0)
    {
        logger.Error("Tests", "> Tests failed: {}", mFailedTests);
    }
    else
    {
        logger.Info("Tests", "> Tests failed: {}", mFailedTests);
    }
}

} // ignite::test.
