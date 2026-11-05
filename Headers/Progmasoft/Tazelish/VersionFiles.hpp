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

    constexpr std::string_view kProjectFileName = "tazelish.txt";
    constexpr std::string_view kStateDirectoryName = ".tazelish";
    constexpr std::string_view kConfigFileName = "config.txt";

    /// A version file that could not be read or written, and why.
    struct FileError
    {
        std::filesystem::path file;
        std::string message;
    };

    /// Which file named the version.
    enum class VersionSource
    {
        kProject,
        kUser,
    };

    /// The version to run and the file that named it.
    struct Selection
    {
        Version version;
        std::filesystem::path file;
        VersionSource source = VersionSource::kProject;
    };

    /// No file names a version.
    struct NoSelection
    {};

    /// The home folder of the user: `%USERPROFILE%` on Windows, `$HOME`
    /// elsewhere; nothing when the variable is unset or empty.
    [[nodiscard]] std::optional<std::filesystem::path>
    HomeDirectory();

    /// `<home>/.tazelish`, where Tazelish keeps what it owns.
    [[nodiscard]] std::filesystem::path
    StateDirectory(const std::filesystem::path &home);

    /// `<home>/.tazelish/config.txt`.
    [[nodiscard]] std::filesystem::path
    ConfigFile(const std::filesystem::path &home);

    /// The version [file] names; NoSelection when it does not exist.
    [[nodiscard]] std::variant<NoSelection, Version, FileError>
    ReadVersionFile(const std::filesystem::path &file);

    /// The version to run in [directory]: the nearest `tazelish.txt` in it or a
    /// folder above it, else the `config.txt` of [home].
    [[nodiscard]] std::variant<NoSelection, Selection, FileError>
    SelectVersion(const std::filesystem::path &directory,
                  const std::optional<std::filesystem::path> &home);

    /// Writes [version] to [file], creating its folder. An existing file is an
    /// error unless [force] is set.
    [[nodiscard]] std::optional<FileError>
    WriteVersionFile(const std::filesystem::path &file,
                     const Version &version,
                     bool force);

} // namespace Progmasoft::Tazelish
