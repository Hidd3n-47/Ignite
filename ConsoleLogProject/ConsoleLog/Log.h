#pragma once

#include <print>
#include <string>
#include <format>
#include <chrono>

namespace ignite
{

class Log
{
public:
    static void Init();
    static void Destroy();

    template <typename... Args>
    inline static void Info(const std::string title, const std::string msg, Args&&... args)
    {
        Print(msg, title, GREEN, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline static void Debug(const std::string title, const std::string msg, Args&&... args)
    {
        Print(msg, title, BLUE, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline static void Warn(const std::string title, const std::string msg, Args&&... args)
    {
        Print(msg, title, ORANGE, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline static void Error(const std::string title, const std::string msg, Args&&... args)
    {
        Print(msg, title, RED, std::forward<Args>(args)...);
    }

private:
    static constexpr const char* GREEN      = "\033[32m";
    static constexpr const char* BLUE       = "\033[94m";
    static constexpr const char* ORANGE     = "\033[38;5;208m";
    static constexpr const char* RED        = "\033[31m";
    static constexpr const char* DEFAULT    = "\033[0m";

    template <typename T>
    inline static auto ToFormatArg(T&& t)
    {
        if constexpr (std::is_convertible_v<T, std::string>)
        {
            return std::string(t);
        }
        else
        {
            return std::forward<T>(t);
        }
    }

    template <typename... Args>
    inline static void Print(const std::string msg, const std::string title, const char* color, Args&&... args)
    {
        const auto now = std::chrono::system_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        const std::string timestamp = std::format(" [{:%H:%M:%S.}{:03}]", std::chrono::floor<std::chrono::seconds>(now), ms.count());

        if constexpr (sizeof...(args) == 0)
        {
            //std::println("{}{}{}: {}{}", color, title, timestamp, msg, DEFAULT);
        }
        else
        {
            auto argsTuple = std::make_tuple(ToFormatArg(std::forward<Args>(args))...);

            auto formattedMsg = std::vformat(msg, std::apply([](auto&... vals) { return std::make_format_args(vals...); }, argsTuple));

            //std::println("{}{}{}: {}{}",  color, title, timestamp, formattedMsg, DEFAULT);
        }
    }

};

} // Namespace ignite.