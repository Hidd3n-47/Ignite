#pragma once

#include <vector>
#include <unordered_map>

#include "Test.h"
#include "Logger/Log.h"

namespace ignite::test
{

/**
 * @class TestRegistry:
 * A class used to register test methods for when running unit tests. <br/>
 * In order to have a project for unit tests, link to this and then <br/>
 * have a local class that inherits from \c TestRegistry and <br/>
 * implement external \c CreateTestEnvironment to return local class. <br/>
 */
class TestRegistry
{
public:
    TestRegistry()              = default;
    virtual ~TestRegistry()     = default;

    TestRegistry(const TestRegistry&)               = delete;
    TestRegistry(TestRegistry&&)                    = delete;
    TestRegistry& operator=(TestRegistry&&)         = delete;
    TestRegistry& operator=(const TestRegistry&)    = delete;

    /**
     * Initialize the test environment.
     */
    void Init();
    /**
     * 'Run' all registered tasks.
     */
    void RunTests();

    /**
     * Initialize the test session.<br/>
     * Override this method to implement session specific variables such as the session name.
     */
    virtual void InitTestSession()  = 0;
    /**
     * Register the tests for the session.
     */
    virtual void RegisterTests()    = 0;

    /**
     * Add a test case to the test session.
     * @param category The category that the test is a part of.
     * @param testName The name of the specific test.
     * @param test A function pointer to the test.
     */
    inline void AddTestCase(const std::string_view category, const std::string_view testName, const std::function<std::optional<std::string>()>& test) { mTests[category].emplace_back(testName, test); }

    inline void SetOnTestFailedCallback(const std::function<void()>& onTestFailedCallback) { mOnTestFailedCallback = onTestFailedCallback; }
protected:
    Log mLogger{ "UnitTests" };
    std::string_view mTestSessionName;
private:
    std::unordered_map<std::string_view, std::vector<Test>> mTests;
    std::function<void()> mOnTestFailedCallback;

    uint32_t mPassedTests = 0;
    uint32_t mFailedTests = 0;

};

TestRegistry* CreateTestEnvironment();

} // Namespace ignite::test.
