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
        /// The version to install.
        Version version;
    };

    /// `tazelish use [--force] <version>`: writes `./tazelish.txt`.
    struct Use
    {
        /// The version the project runs.
        Version version;
        /// Whether an existing `tazelish.txt` is replaced.
        bool force = false;
    };

    /// `tazelish default [--force] <version>`: writes the `config.txt` of the
    /// user.
    struct Default
    {
        /// The version the user runs outside a project.
        Version version;
        /// Whether an existing `config.txt` is replaced.
        bool force = false;
    };

    /// What `tazelish self` does to Tazelish itself.
    enum class SelfAction
    {
        /// Update Tazelish.
        kUpdate,
        /// Remove Tazelish and every version of Tazel it installed, and undo
        /// the `PATH` entries it added.
        kRemove,
        /// Do what kRemove does, and also delete what Tazelish kept for itself:
        /// its `config.txt`, its download cache and its logs.
        kPurge,
    };

    /// `tazelish self update|remove|purge`.
    struct Self
    {
        /// What to do.
        SelfAction action = SelfAction::kUpdate;
    };

    /// The commands `tazelish help <command>` explains.
    enum class Topic
    {
        /// `tazelish help install`.
        kInstall,
        /// `tazelish help use`.
        kUse,
        /// `tazelish help default`.
        kDefault,
        /// `tazelish help self`.
        kSelf,
        /// `tazelish help help`.
        kHelp,
    };

    /// `tazelish help [command]`; without a topic, the general help.
    struct Help
    {
        /// The command to explain; nothing for the general help.
        std::optional<Topic> topic;
    };

    /// A command line that Tazelish accepts.
    using Command = std::variant<Install, Use, Default, Self, Help>;

    /// A command line that is wrong, and what is wrong with it.
    struct Rejection
    {
        /// What is wrong, in a sentence for the user.
        std::string message;
    };

    /// The command a command line asks for, or why it asks for none.
    using ParseResult = std::variant<Command, Rejection>;

    /// The command [arguments] ask for; [arguments] leave out the name of the
    /// program. No arguments ask for the general help.
    /// @param arguments The arguments after the name of the program.
    /// @return The command, or the rejection of the command line.
    [[nodiscard]] ParseResult
    ParseCommandLine(const std::vector<std::string_view> &arguments);

    /// The name of a topic, as `tazelish help` takes it.
    /// @param topic The topic.
    /// @return Its name, such as `install`.
    [[nodiscard]] std::string_view
    TopicName(Topic topic);

} // namespace Progmasoft::Tazelish
