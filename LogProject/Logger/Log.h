#pragma once

#include <print>
#include <chrono>
#include <fstream>
#include <iostream>

namespace ignite
{
class Log
{
public:
    Log(std::string title)
        : mTitle(std::move(title))
        , mFileOutput(mTitle + ".log")
    {
        if (mFileOutput.fail())
        {
            std::cout << RED << "Failed to create log files for '{}'. Messages might not be logged to disk correctly." << DEFAULT << "\n";
        }
    }

    ~Log()
    {
        mFileOutput.close();
    }

    template <typename... Args>
    inline void Info(const std::string_view msg, Args&&... args)
    {
        Print(msg, GREEN, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void Debug(const std::string_view msg, Args&&... args)
    {
        Print(msg, BLUE, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void Warn(const std::string_view msg, Args&&... args)
    {
        Print(msg, ORANGE, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void Error(const std::string_view msg, Args&&... args)
    {
        Print(msg, RED, std::forward<Args>(args)...);
    }

private:
    static constexpr const char* GREEN      = "\033[32m";
    static constexpr const char* BLUE       = "\033[94m";
    static constexpr const char* ORANGE     = "\033[38;5;208m";
    static constexpr const char* RED        = "\033[31m";
    static constexpr const char* DEFAULT    = "\033[0m";

    std::string   mTitle;
    std::ofstream mFileOutput;

    inline static std::string ColorToMessageLevel(const char* color)
    {
        if (color == GREEN)
        {
            return "INFO  | ";
        }
        if (color == BLUE)
        {
            return "DEBUG | ";
        }
        if (color == ORANGE)
        {
            return "WARN  | ";
        }
        if (color == RED)
        {
            return "ERROR | ";
        }

        return "     | ";
    }

    template <typename T>
    inline static auto ToFormatArg(T&& t)
    {
        if constexpr (std::is_convertible_v<T, std::string_view>)
        {
            return std::string(t);
        }
        else
        {
            return std::forward<T>(t);
        }
    }

    template <typename... Args>
    inline void Print(const std::string_view msg, const char* color, Args&&... args)
    {
        const auto now = std::chrono::system_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        const std::string timestamp = std::format(" [{:%H:%M:%S.}{:03}]", std::chrono::floor<std::chrono::seconds>(now), ms.count());

        if constexpr (sizeof...(args) == 0)
        {
            std::println("{}{}{}: {}{}", color, mTitle, timestamp, msg, DEFAULT);

            mFileOutput << ColorToMessageLevel(color) << mTitle << timestamp << ": " << msg << '\n';
        }
        else
        {
            auto argsTuple = std::make_tuple(ToFormatArg(std::forward<Args>(args))...);

            auto formattedMsg = std::vformat(msg, std::apply([](auto&... vals) { return std::make_format_args(vals...); }, argsTuple));

            std::println("{}{}{}: {}{}", color, mTitle, timestamp, formattedMsg, DEFAULT);

            mFileOutput << ColorToMessageLevel(color) << mTitle << timestamp << ": " << formattedMsg << '\n';
        }
    }

};

} // Namespace ignite.