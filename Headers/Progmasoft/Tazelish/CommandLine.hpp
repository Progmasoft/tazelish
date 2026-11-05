// SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com>
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

/// \file
/// The command line of Tazelish, read into a typed command. Nothing after the
/// parser looks at the text of an argument again.

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "Progmasoft/Tazelish/Version.hpp"

namespace Progmasoft::Tazelish
{

    /// `tazelish install <version>`.
    struct Install
    {
        Version version;
    };

    /// `tazelish use [--force] <version>`: writes `./tazelish.txt`.
    struct Use
    {
        Version version;
        bool force = false;
    };

    /// `tazelish default [--force] <version>`: writes the `config.txt` of the
    /// user.
    struct Default
    {
        Version version;
        bool force = false;
    };

    /// What `tazelish self` does to Tazelish itself.
    enum class SelfAction
    {
        kUpdate,
        kRemove,
        kPurge,
    };

    /// `tazelish self update|remove|purge`.
    struct Self
    {
        SelfAction action = SelfAction::kUpdate;
    };

    /// The commands `tazelish help <command>` explains.
    enum class Topic
    {
        kInstall,
        kUse,
        kDefault,
        kSelf,
        kHelp,
    };

    /// `tazelish help [command]`; without a topic, the general help.
    struct Help
    {
        std::optional<Topic> topic;
    };

    using Command = std::variant<Install, Use, Default, Self, Help>;

    /// A command line that is wrong, and what is wrong with it.
    struct Rejection
    {
        std::string message;
    };

    using ParseResult = std::variant<Command, Rejection>;

    /// The command [arguments] ask for; [arguments] leave out the name of the
    /// program. No arguments ask for the general help.
    [[nodiscard]] ParseResult
    ParseCommandLine(const std::vector<std::string_view> &arguments);

    /// The name of a topic, as `tazelish help` takes it.
    [[nodiscard]] std::string_view
    TopicName(Topic topic);

} // namespace Progmasoft::Tazelish
