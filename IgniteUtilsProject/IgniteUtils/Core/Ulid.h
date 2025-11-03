#pragma once

#include <set>
#include <cstdint>

namespace ignite
{
/**
 * @class Ulid: A class that represents a 64 bit Unique Lexicographically Sortable Identifier.
 * The first 32 bits represent the time since epoch (01/01/2025) of creation.
 * The second 32 bits represent a random number.
 */
class Ulid
{
public:
    Ulid();
    constexpr explicit Ulid(const uint64_t ulid) : mUlid{ ulid } { /* Empty. */ }

    inline operator uint64_t() const
    {
        return mUlid;
    }

    inline bool operator==(const Ulid& other) const
    {
        return mUlid == other.mUlid;
    }
private:
    uint64_t mUlid;

    [[nodiscard]] static uint32_t GetTimeFromEpoch();

    [[nodiscard]] static uint32_t GetRandomNumber();
};

} // Namespace ignite.

template <>
struct std::hash<ignite::Ulid>
{
    [[nodiscard]] inline size_t operator()(const ignite::Ulid& ulid) const noexcept
    {
        return std::hash<uint64_t>()(ulid);
    }
};