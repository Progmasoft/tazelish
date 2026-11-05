// SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com>
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <Progmasoft/Catch3.hpp>

#include "Progmasoft/Tazelish/Version.hpp"

namespace tazelish = Progmasoft::Tazelish;

TEST_CASE("A release has four numbers without leading zeros", "[version]")
{
    const auto version = tazelish::ParseVersion("0.4.0.3");
    REQUIRE(version.has_value());
    CHECK(std::get<tazelish::Release>(*version)
          == tazelish::Release{ 0, 4, 0, 3 });
    CHECK(tazelish::FormatVersion(*version) == "0.4.0.3");
    CHECK(tazelish::ParseVersion("12.19.9.9").has_value());

    for (const char *text : { "0.4.0",
                              "0.4.0.3.1",
                              "0.04.0.3",
                              "0.4..3",
                              ".0.4.0",
                              "0.4.0.",
                              "a.4.0.3",
                              "0.4.0.3 ",
                              "4294967296.0.0.0",
                              "" })
    {
        INFO(text);
        CHECK_FALSE(tazelish::ParseVersion(text).has_value());
    }
}

TEST_CASE("A channel names its newest build", "[version]")
{
    CHECK(std::get<tazelish::Newest>(*tazelish::ParseVersion("unstable"))
          == tazelish::Newest{ tazelish::Channel::kUnstable });
    CHECK(std::get<tazelish::Newest>(*tazelish::ParseVersion("nightly"))
          == tazelish::Newest{ tazelish::Channel::kNightly });
    CHECK_FALSE(tazelish::ParseVersion("Nightly").has_value());
    CHECK_FALSE(tazelish::ParseVersion("stable").has_value());
}

TEST_CASE("A dated build needs a day the calendar has", "[version]")
{
    const auto version = tazelish::ParseVersion("2026-11-05-nightly");
    REQUIRE(version.has_value());
    CHECK(std::get<tazelish::Dated>(*version)
          == tazelish::Dated{ 2026, 11, 5, tazelish::Channel::kNightly });
    CHECK(tazelish::FormatVersion(*version) == "2026-11-05-nightly");
    CHECK(tazelish::ParseVersion("2028-02-29-unstable").has_value());

    for (const char *text : { "2026-02-29-unstable",
                              "1900-02-29-nightly",
                              "2026-13-01-nightly",
                              "2026-00-10-nightly",
                              "2026-04-31-nightly",
                              "2026-11-5-nightly",
                              "2026-11-05",
                              "2026-11-05-",
                              "2026-11-05-stable",
                              "2026/11/05-nightly" })
    {
        INFO(text);
        CHECK_FALSE(tazelish::ParseVersion(text).has_value());
    }
}
