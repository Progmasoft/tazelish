// SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com>
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

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
        std::uint32_t majorNumber = 0;
        std::uint32_t semimajorNumber = 0;
        std::uint32_t minorNumber = 0;
        std::uint32_t patchNumber = 0;
    };

    /// The two kinds of build that are not releases.
    enum class Channel
    {
        kUnstable,
        kNightly,
    };

    /// The newest build of a channel: `unstable` or `nightly`.
    struct Newest
    {
        Channel channel = Channel::kUnstable;
    };

    /// The build of a channel from one day: `YYYY-MM-DD-unstable`.
    struct Dated
    {
        std::uint16_t year = 0;
        std::uint8_t month = 0;
        std::uint8_t day = 0;
        Channel channel = Channel::kUnstable;
    };

    [[nodiscard]] inline bool
    operator==(const Release &left, const Release &right)
    {
        return left.majorNumber == right.majorNumber
               && left.semimajorNumber == right.semimajorNumber
               && left.minorNumber == right.minorNumber
               && left.patchNumber == right.patchNumber;
    }

    [[nodiscard]] inline bool
    operator==(const Newest &left, const Newest &right)
    {
        return left.channel == right.channel;
    }

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
    [[nodiscard]] std::optional<Version>
    ParseVersion(std::string_view text);

    /// The text of [version], the way ParseVersion reads it.
    [[nodiscard]] std::string
    FormatVersion(const Version &version);

} // namespace Progmasoft::Tazelish
