// SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com>
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <Progmasoft/Catch3.hpp>
#include <string>

#include "Progmasoft/Tazelish/CommandLine.hpp"

namespace tazelish = Progmasoft::Tazelish;

namespace
{

    tazelish::Command
    Parsed(const std::vector<std::string_view> &arguments)
    {
        auto result = tazelish::ParseCommandLine(arguments);
        REQUIRE(std::holds_alternative<tazelish::Command>(result));
        return std::get<tazelish::Command>(result);
    }

    std::string
    Rejected(const std::vector<std::string_view> &arguments)
    {
        auto result = tazelish::ParseCommandLine(arguments);
        REQUIRE(std::holds_alternative<tazelish::Rejection>(result));
        return std::get<tazelish::Rejection>(result).message;
    }

    bool
    Contains(const std::string &text, const char *part)
    {
        return text.find(part) != std::string::npos;
    }

} // namespace

TEST_CASE("install takes one version", "[command-line]")
{
    const auto command = Parsed({ "install", "2026-11-05-unstable" });
    CHECK(std::get<tazelish::Install>(command).version
          == tazelish::Version{
              tazelish::Dated{ 2026, 11, 5, tazelish::Channel::kUnstable } });
    CHECK(Contains(Rejected({ "install" }), "one version"));
    CHECK(Contains(Rejected({ "install", "1.0" }), "is not a version"));
    CHECK(Contains(Rejected({ "install", "nightly", "unstable" }),
                   "one version"));
}

TEST_CASE("use and default take --force before or after the version",
          "[command-line]")
{
    CHECK_FALSE(std::get<tazelish::Use>(Parsed({ "use", "0.4.0.3" })).force);
    CHECK(
        std::get<tazelish::Use>(Parsed({ "use", "--force", "nightly" })).force);
    CHECK(
        std::get<tazelish::Default>(Parsed({ "default", "nightly", "--force" }))
            .force);

    CHECK(Contains(Rejected({ "use", "--force", "--force", "nightly" }),
                   "twice"));
    CHECK(Contains(Rejected({ "default", "-f", "nightly" }), "not an option"));
    CHECK(Contains(Rejected({ "default", "--force" }), "takes a version"));
    CHECK(Contains(Rejected({ "use", "nightly", "unstable" }), "one version"));
}

TEST_CASE("self takes update, remove or purge", "[command-line]")
{
    CHECK(std::get<tazelish::Self>(Parsed({ "self", "update" })).action
          == tazelish::SelfAction::kUpdate);
    CHECK(std::get<tazelish::Self>(Parsed({ "self", "remove" })).action
          == tazelish::SelfAction::kRemove);
    CHECK(std::get<tazelish::Self>(Parsed({ "self", "purge" })).action
          == tazelish::SelfAction::kPurge);
    CHECK(Contains(Rejected({ "self" }), "update, remove or purge"));
    CHECK(
        Contains(Rejected({ "self", "uninstall" }), "update, remove or purge"));
    CHECK(Contains(Rejected({ "self", "update", "now" }),
                   "update, remove or purge"));
}

TEST_CASE("help names a command or none", "[command-line]")
{
    CHECK_FALSE(std::get<tazelish::Help>(Parsed({})).topic.has_value());
    CHECK_FALSE(std::get<tazelish::Help>(Parsed({ "help" })).topic.has_value());
    CHECK(std::get<tazelish::Help>(Parsed({ "help", "default" })).topic
          == tazelish::Topic::kDefault);
    CHECK(tazelish::TopicName(tazelish::Topic::kSelf) == "self");
    CHECK(Contains(Rejected({ "help", "fly" }), "not a command"));
    CHECK(Contains(Rejected({ "instal", "nightly" }), "not a command"));
}
