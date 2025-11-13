#pragma once

#include <string>
#include <optional>
#include <functional>
#include <string_view>

namespace ignite::test
{

/**
 * @class Test
 * A class to represent the data of a unit test.
 */
class Test
{
public:
    /**
     * Create a test.
     * @param testName The name of the test.
     * @param test A function pointer to the test method.
     */
    inline Test(const std::string_view testName, const std::function<std::optional<std::string>()>& test)
        : mTestName(testName)
        , mTest(test)
    { /* Empty. */ }

    /**
     * @brief Run the test.
     * @return An optional that is black if the test passed, and is the reason of test failure if it failed.
     */
    [[nodiscard]] inline std::optional<std::string> Run() const { return mTest(); }

    /** Get the name of the test. */
    [[nodiscard]] inline std::string_view GetTestName() const { return mTestName; }
private:
    std::string_view mTestName;
    std::function<std::optional<std::string>()> mTest;
};

} // Namespace ignite::test.
