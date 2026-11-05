// SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com>
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <array>
#include <fmt/format.h>
#include <utility>

#include "Progmasoft/Tazelish/CommandLine.hpp"

namespace Progmasoft::Tazelish
{
    namespace
    {

        constexpr std::array<std::pair<std::string_view, Topic>, 5> kTopics
            = { {
                { "install", Topic::kInstall },
                { "use", Topic::kUse },
                { "default", Topic::kDefault },
                { "self", Topic::kSelf },
                { "help", Topic::kHelp },
            } };

        constexpr std::string_view kVersionForms
            = "MAJOR.SEMIMAJOR.MINOR.PATCH, unstable, nightly, "
              "YYYY-MM-DD-unstable or "
              "YYYY-MM-DD-nightly";

        Rejection
        Reject(std::string message)
        {
            return Rejection{ std::move(message) };
        }

        // The one version of a command, written as the argument [text].
        std::variant<Version, Rejection>
        ReadVersion(std::string_view command, std::string_view text)
        {
            if (auto version = ParseVersion(text))
            {
                return *version;
            }
            return Reject(
                fmt::format("'{}' is not a version; tazelish {} takes {}",
                            text,
                            command,
                            kVersionForms));
        }

        // `install <version>`.
        ParseResult
        ParseInstall(const std::vector<std::string_view> &rest)
        {
            if (rest.size() != 1)
            {
                return Reject("tazelish install takes one version, such as "
                              "tazelish install 0.4.0.3");
            }
            auto version = ReadVersion("install", rest.front());
            if (auto *rejection = std::get_if<Rejection>(&version))
            {
                return std::move(*rejection);
            }
            return Command{ Install{ std::get<Version>(version) } };
        }

        // `use|default [--force] <version>`, the flag before or after the
        // version.
        template<typename Writing>
        ParseResult
        ParseWriting(std::string_view command,
                     const std::vector<std::string_view> &rest)
        {
            bool force = false;
            std::optional<std::string_view> text;
            for (const std::string_view argument : rest)
            {
                if (argument == "--force")
                {
                    if (force)
                    {
                        return Reject(fmt::format(
                            "--force is written twice in tazelish {}",
                            command));
                    }
                    force = true;
                }
                else if (argument.size() > 1 && argument.front() == '-')
                {
                    return Reject(fmt::format("'{}' is not an option of "
                                              "tazelish {}; it takes --force",
                                              argument,
                                              command));
                }
                else if (text)
                {
                    return Reject(
                        fmt::format("tazelish {} takes one version", command));
                }
                else
                {
                    text = argument;
                }
            }
            if (!text)
            {
                return Reject(fmt::format(
                    "tazelish {} takes a version, such as tazelish {} 0.4.0.3",
                    command,
                    command));
            }
            auto version = ReadVersion(command, *text);
            if (auto *rejection = std::get_if<Rejection>(&version))
            {
                return std::move(*rejection);
            }
            return Command{ Writing{ std::get<Version>(version), force } };
        }

        // `self update|remove|purge`.
        ParseResult
        ParseSelf(const std::vector<std::string_view> &rest)
        {
            if (rest.size() == 1)
            {
                if (rest.front() == "update")
                {
                    return Command{ Self{ SelfAction::kUpdate } };
                }
                if (rest.front() == "remove")
                {
                    return Command{ Self{ SelfAction::kRemove } };
                }
                if (rest.front() == "purge")
                {
                    return Command{ Self{ SelfAction::kPurge } };
                }
            }
            return Reject("tazelish self takes one of update, remove or purge");
        }

        // `help [command]`.
        ParseResult
        ParseHelp(const std::vector<std::string_view> &rest)
        {
            if (rest.empty())
            {
                return Command{ Help{} };
            }
            if (rest.size() == 1)
            {
                for (const auto &[name, topic] : kTopics)
                {
                    if (rest.front() == name)
                    {
                        return Command{ Help{ topic } };
                    }
                }
                return Reject(
                    fmt::format("'{}' is not a command of tazelish; write "
                                "tazelish help for the commands",
                                rest.front()));
            }
            return Reject("tazelish help takes at most one command");
        }

    } // namespace

    ParseResult
    ParseCommandLine(const std::vector<std::string_view> &arguments)
    {
        if (arguments.empty())
        {
            return Command{ Help{} };
        }
        const std::string_view command = arguments.front();
        const std::vector<std::string_view> rest(arguments.begin() + 1,
                                                 arguments.end());
        if (command == "install")
        {
            return ParseInstall(rest);
        }
        if (command == "use")
        {
            return ParseWriting<Use>(command, rest);
        }
        if (command == "default")
        {
            return ParseWriting<Default>(command, rest);
        }
        if (command == "self")
        {
            return ParseSelf(rest);
        }
        if (command == "help")
        {
            return ParseHelp(rest);
        }
        return Reject(fmt::format(
            "'{}' is not a command of tazelish; write tazelish help for the "
            "commands",
            command));
    }

    std::string_view
    TopicName(Topic topic)
    {
        for (const auto &[name, candidate] : kTopics)
        {
            if (candidate == topic)
            {
                return name;
            }
        }
        return {};
    }

} // namespace Progmasoft::Tazelish
