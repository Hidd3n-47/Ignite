#pragma once

#include <print>
#include <string>
#include <format>
#include <chrono>
#include <fstream>
#include <unordered_map>

namespace ignite
{

class Log
{
public:
    template <typename... Args>
    inline static void Info(const std::string_view title, const std::string_view msg, Args&&... args)
    {
        Print(msg, title, GREEN, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline static void Debug(const std::string_view title, const std::string_view msg, Args&&... args)
    {
        Print(msg, title, BLUE, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline static void Warn(const std::string_view title, const std::string_view msg, Args&&... args)
    {
        Print(msg, title, ORANGE, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline static void Error(const std::string_view title, const std::string_view msg, Args&&... args)
    {
        Print(msg, title, RED, std::forward<Args>(args)...);
    }

private:
    static constexpr const char* GREEN      = "\033[32m";
    static constexpr const char* BLUE       = "\033[94m";
    static constexpr const char* ORANGE     = "\033[38;5;208m";
    static constexpr const char* RED        = "\033[31m";
    static constexpr const char* DEFAULT    = "\033[0m";

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

    inline static std::unordered_map<std::string, std::ofstream> mLogNameToFileOutput{};

    template <typename... Args>
    inline static void Print(const std::string_view msg, const std::string_view title, const char* color, Args&&... args)
    {
        const std::string logPath = std::string{ title } + ".log";
        if (!mLogNameToFileOutput.contains(logPath))
        {
            mLogNameToFileOutput[logPath] = std::ofstream(logPath);

            if (mLogNameToFileOutput[logPath].fail())
            {
                std::println("Failed to create log files for '{}'. Messages might not be logged to disk correctly.", title);
            }
        }

        const auto now = std::chrono::system_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        const std::string timestamp = std::format(" [{:%H:%M:%S.}{:03}]", std::chrono::floor<std::chrono::seconds>(now), ms.count());

        if constexpr (sizeof...(args) == 0)
        {
            std::println("{}{}{}: {}{}", color, title, timestamp, msg, DEFAULT);

            mLogNameToFileOutput[logPath] << ColorToMessageLevel(color) << title << timestamp << ": " << msg << '\n';
        }
        else
        {
            auto argsTuple = std::make_tuple(ToFormatArg(std::forward<Args>(args))...);

            auto formattedMsg = std::vformat(msg, std::apply([](auto&... vals) { return std::make_format_args(vals...); }, argsTuple));

            std::println("{}{}{}: {}{}",  color, title, timestamp, formattedMsg, DEFAULT);

            mLogNameToFileOutput[logPath] << ColorToMessageLevel(color) << title << timestamp << ": " << formattedMsg << '\n';
        }
    }

};

} // Namespace ignite.