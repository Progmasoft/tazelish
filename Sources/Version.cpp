// SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com>
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <array>
#include <fmt/format.h>
#include <limits>

#include "Progmasoft/Tazelish/Version.hpp"

namespace Progmasoft::Tazelish
{
    namespace
    {

        constexpr std::string_view kUnstable = "unstable";
        constexpr std::string_view kNightly = "nightly";

        std::optional<Channel>
        ParseChannel(std::string_view text)
        {
            if (text == kUnstable)
            {
                return Channel::kUnstable;
            }
            if (text == kNightly)
            {
                return Channel::kNightly;
            }
            return std::nullopt;
        }

        std::string_view
        ChannelName(Channel channel)
        {
            return channel == Channel::kUnstable ? kUnstable : kNightly;
        }

        bool
        IsDigit(char character)
        {
            return character >= '0' && character <= '9';
        }

        // A number of digits only, without a leading zero unless it is zero.
        std::optional<std::uint32_t>
        ParseNumber(std::string_view text)
        {
            if (text.empty() || (text.size() > 1 && text.front() == '0'))
            {
                return std::nullopt;
            }
            std::uint64_t value = 0;
            for (const char character : text)
            {
                if (!IsDigit(character))
                {
                    return std::nullopt;
                }
                value
                    = value * 10 + static_cast<std::uint64_t>(character - '0');
                if (value > std::numeric_limits<std::uint32_t>::max())
                {
                    return std::nullopt;
                }
            }
            return static_cast<std::uint32_t>(value);
        }

        std::optional<Release>
        ParseRelease(std::string_view text)
        {
            std::array<std::uint32_t, 4> numbers{};
            for (std::size_t index = 0; index < numbers.size(); ++index)
            {
                const std::size_t dot = text.find('.');
                const bool last = index + 1 == numbers.size();
                if (last != (dot == std::string_view::npos))
                {
                    return std::nullopt;
                }
                const auto number = ParseNumber(text.substr(0, dot));
                if (!number)
                {
                    return std::nullopt;
                }
                numbers[index] = *number;
                text = last ? std::string_view{} : text.substr(dot + 1);
            }
            return Release{ numbers[0], numbers[1], numbers[2], numbers[3] };
        }

        // Exactly [width] digits; leading zeros belong to a date.
        std::optional<std::uint32_t>
        ParseFixed(std::string_view text, std::size_t width)
        {
            if (text.size() != width)
            {
                return std::nullopt;
            }
            std::uint32_t value = 0;
            for (const char character : text)
            {
                if (!IsDigit(character))
                {
                    return std::nullopt;
                }
                value
                    = value * 10 + static_cast<std::uint32_t>(character - '0');
            }
            return value;
        }

        bool
        IsLeapYear(std::uint32_t year)
        {
            return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
        }

        std::uint32_t
        DaysInMonth(std::uint32_t year, std::uint32_t month)
        {
            constexpr std::array<std::uint32_t, 12> kDays
                = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
            return month == 2 && IsLeapYear(year) ? 29 : kDays[month - 1];
        }

        // YYYY-MM-DD-channel, with a date the calendar has.
        std::optional<Dated>
        ParseDated(std::string_view text)
        {
            constexpr std::size_t kDateLength = 10;
            if (text.size() <= kDateLength + 1 || text[4] != '-'
                || text[7] != '-' || text[kDateLength] != '-')
            {
                return std::nullopt;
            }
            const auto year = ParseFixed(text.substr(0, 4), 4);
            const auto month = ParseFixed(text.substr(5, 2), 2);
            const auto day = ParseFixed(text.substr(8, 2), 2);
            const auto channel = ParseChannel(text.substr(kDateLength + 1));
            if (!year || !month || !day || !channel || *year == 0 || *month == 0
                || *month > 12 || *day == 0
                || *day > DaysInMonth(*year, *month))
            {
                return std::nullopt;
            }
            return Dated{ static_cast<std::uint16_t>(*year),
                          static_cast<std::uint8_t>(*month),
                          static_cast<std::uint8_t>(*day),
                          *channel };
        }

    } // namespace

    std::optional<Version>
    ParseVersion(std::string_view text)
    {
        if (const auto channel = ParseChannel(text))
        {
            return Newest{ *channel };
        }
        if (const auto release = ParseRelease(text))
        {
            return *release;
        }
        if (const auto dated = ParseDated(text))
        {
            return *dated;
        }
        return std::nullopt;
    }

    std::string
    FormatVersion(const Version &version)
    {
        if (const auto *release = std::get_if<Release>(&version))
        {
            return fmt::format("{}.{}.{}.{}",
                               release->majorNumber,
                               release->semimajorNumber,
                               release->minorNumber,
                               release->patchNumber);
        }
        if (const auto *newest = std::get_if<Newest>(&version))
        {
            return std::string(ChannelName(newest->channel));
        }
        const auto &dated = std::get<Dated>(version);
        return fmt::format("{:04}-{:02}-{:02}-{}",
                           dated.year,
                           dated.month,
                           dated.day,
                           ChannelName(dated.channel));
    }

} // namespace Progmasoft::Tazelish
