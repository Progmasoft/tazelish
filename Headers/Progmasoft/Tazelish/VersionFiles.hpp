// SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com>
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

/// \file
/// The files that name the version of Tazel to run: `tazelish.txt` of a
/// project, written by `tazelish use`, and `config.txt` of the user, written
/// by `tazelish default`. Both hold one version; `tazelish.txt` comes first.

#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include "Progmasoft/Tazelish/Version.hpp"

namespace Progmasoft::Tazelish
{

    /// The file of a project that names its version of Tazel.
    constexpr std::string_view kProjectFileName = "tazelish.txt";
    /// The folder in the home of the user where Tazelish keeps what it owns.
    constexpr std::string_view kStateDirectoryName = ".tazelish";
    /// The file in that folder that names the version of the user.
    constexpr std::string_view kConfigFileName = "config.txt";

    /// A version file that could not be read or written, and why.
    struct FileError
    {
        /// The file.
        std::filesystem::path file;
        /// What went wrong, in a sentence for the user.
        std::string message;
    };

    /// Which file named the version.
    enum class VersionSource
    {
        /// The `tazelish.txt` of a project.
        kProject,
        /// The `config.txt` of the user.
        kUser,
    };

    /// The version to run and the file that named it.
    struct Selection
    {
        /// The version to run.
        Version version;
        /// The file that named it.
        std::filesystem::path file;
        /// Which kind of file that is.
        VersionSource source = VersionSource::kProject;
    };

    /// No file names a version.
    struct NoSelection
    {};

    /// The home folder of the user: `%USERPROFILE%` on Windows, `$HOME`
    /// elsewhere; nothing when the variable is unset or empty.
    /// @return The folder, or nothing.
    [[nodiscard]] std::optional<std::filesystem::path>
    HomeDirectory();

    /// `<home>/.tazelish`, where Tazelish keeps what it owns.
    /// @param home The home folder of the user.
    /// @return The folder.
    [[nodiscard]] std::filesystem::path
    StateDirectory(const std::filesystem::path &home);

    /// `<home>/.tazelish/config.txt`.
    /// @param home The home folder of the user.
    /// @return The file.
    [[nodiscard]] std::filesystem::path
    ConfigFile(const std::filesystem::path &home);

    /// The version [file] names; NoSelection when it does not exist.
    /// @param file The version file.
    /// @return The version, NoSelection, or why the file could not be read.
    [[nodiscard]] std::variant<NoSelection, Version, FileError>
    ReadVersionFile(const std::filesystem::path &file);

    /// The version to run in [directory]: the nearest `tazelish.txt` in it or a
    /// folder above it, else the `config.txt` of [home].
    /// @param directory The folder the command runs in.
    /// @param home The home folder of the user, or nothing.
    /// @return The selection, NoSelection, or why a file could not be read.
    [[nodiscard]] std::variant<NoSelection, Selection, FileError>
    SelectVersion(const std::filesystem::path &directory,
                  const std::optional<std::filesystem::path> &home);

    /// Writes [version] to [file], creating its folder. An existing file is an
    /// error unless [force] is set.
    /// @param file The version file.
    /// @param version The version to write.
    /// @param force Whether an existing file is replaced.
    /// @return Nothing on success, or why the file could not be written.
    [[nodiscard]] std::optional<FileError>
    WriteVersionFile(const std::filesystem::path &file,
                     const Version &version,
                     bool force);

} // namespace Progmasoft::Tazelish
