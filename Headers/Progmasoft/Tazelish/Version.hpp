// SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com>
// SPDX-License-Identifier: MIT

/// \file
/// The versions of Tazel a command line or a version file can name.

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

namespace Progmasoft::Tazelish
{

    /// A release of Tazel: `MAJOR.SEMIMAJOR.MINOR.PATCH`.
    struct Release
    {
        /// The first number.
        std::uint32_t majorNumber = 0;
        /// The second number.
        std::uint32_t semimajorNumber = 0;
        /// The third number.
        std::uint32_t minorNumber = 0;
        /// The fourth number.
        std::uint32_t patchNumber = 0;
    };

    /// The two kinds of build that are not releases.
    enum class Channel
    {
        /// The `unstable` builds.
        kUnstable,
        /// The `nightly` builds.
        kNightly,
    };

    /// The newest build of a channel: `unstable` or `nightly`.
    struct Newest
    {
        /// The channel.
        Channel channel = Channel::kUnstable;
    };

    /// The build of a channel from one day: `YYYY-MM-DD-unstable`.
    struct Dated
    {
        /// The year, such as 2026.
        std::uint16_t year = 0;
        /// The month, from 1 to 12.
        std::uint8_t month = 0;
        /// The day of the month, from 1.
        std::uint8_t day = 0;
        /// The channel.
        Channel channel = Channel::kUnstable;
    };

    /// Whether two releases have the same four numbers.
    /// @param left One release.
    /// @param right The other release.
    /// @return true when they are the same release.
    [[nodiscard]] inline bool
    operator==(const Release &left, const Release &right)
    {
        return left.majorNumber == right.majorNumber
               && left.semimajorNumber == right.semimajorNumber
               && left.minorNumber == right.minorNumber
               && left.patchNumber == right.patchNumber;
    }

    /// Whether two newest builds are of the same channel.
    /// @param left One build.
    /// @param right The other build.
    /// @return true when the channels are the same.
    [[nodiscard]] inline bool
    operator==(const Newest &left, const Newest &right)
    {
        return left.channel == right.channel;
    }

    /// Whether two dated builds are of the same day and channel.
    /// @param left One build.
    /// @param right The other build.
    /// @return true when the date and the channel are the same.
    [[nodiscard]] inline bool
    operator==(const Dated &left, const Dated &right)
    {
        return left.year == right.year && left.month == right.month
               && left.day == right.day && left.channel == right.channel;
    }

    /// A version as it is written after `tazelish install`.
    using Version = std::variant<Release, Newest, Dated>;

    /// The version [text] names, or nothing when it names none. Numbers have no
    /// leading zeros, and a date has to exist in the calendar.
    /// @param text The text, such as `unstable` or a release number.
    /// @return The version, or nothing.
    [[nodiscard]] std::optional<Version>
    ParseVersion(std::string_view text);

    /// The text of [version], the way ParseVersion reads it.
    /// @param version The version.
    /// @return Its text.
    [[nodiscard]] std::string
    FormatVersion(const Version &version);

} // namespace Progmasoft::Tazelish
