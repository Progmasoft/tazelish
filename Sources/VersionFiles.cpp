// SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com>
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <system_error>
#include <utility>

#include "Progmasoft/Tazelish/VersionFiles.hpp"

namespace Progmasoft::Tazelish
{
    namespace
    {

        namespace fs = std::filesystem;

        // A version file is short; anything longer names no version.
        constexpr std::size_t kLongestContent = 64;

        FileError
        Fail(const fs::path &file, std::string message)
        {
            return FileError{ file, std::move(message) };
        }

        // The text of a file without the line break and blanks around the
        // version.
        std::string_view
        Trim(std::string_view text)
        {
            constexpr std::string_view kBlanks = " \t\r\n";
            constexpr std::string_view kByteOrderMark = "\xEF\xBB\xBF";
            if (text.substr(0, kByteOrderMark.size()) == kByteOrderMark)
            {
                text.remove_prefix(kByteOrderMark.size());
            }
            const std::size_t first = text.find_first_not_of(kBlanks);
            if (first == std::string_view::npos)
            {
                return {};
            }
            const std::size_t last = text.find_last_not_of(kBlanks);
            return text.substr(first, last - first + 1);
        }

    } // namespace

    std::optional<fs::path>
    HomeDirectory()
    {
#ifdef _WIN32
        constexpr const char *kVariable = "USERPROFILE";
#else
        constexpr const char *kVariable = "HOME";
#endif
        // The environment is read once, before any thread starts.
        const char *value
            = std::getenv(kVariable); // NOLINT(concurrency-mt-unsafe)
        if (value == nullptr || *value == '\0')
        {
            return std::nullopt;
        }
        return fs::path(value);
    }

    fs::path
    StateDirectory(const fs::path &home)
    {
        return home / fs::path(kStateDirectoryName);
    }

    fs::path
    ConfigFile(const fs::path &home)
    {
        return StateDirectory(home) / fs::path(kConfigFileName);
    }

    std::variant<NoSelection, Version, FileError>
    ReadVersionFile(const fs::path &file)
    {
        std::error_code error;
        const fs::file_status status = fs::status(file, error);
        if (status.type() == fs::file_type::not_found)
        {
            return NoSelection{};
        }
        if (error)
        {
            return Fail(file, error.message());
        }
        if (status.type() != fs::file_type::regular)
        {
            return Fail(file, "is not a regular file");
        }
        std::ifstream stream(file, std::ios::binary);
        if (!stream)
        {
            return Fail(file, "cannot be read");
        }
        std::string content;
        content.resize(kLongestContent + 1);
        stream.read(content.data(),
                    static_cast<std::streamsize>(content.size()));
        content.resize(static_cast<std::size_t>(stream.gcount()));
        if (stream.bad())
        {
            return Fail(file, "cannot be read");
        }
        const std::string_view text = Trim(content);
        if (content.size() > kLongestContent)
        {
            return Fail(file, "holds more than a version");
        }
        if (auto version = ParseVersion(text))
        {
            return *version;
        }
        return Fail(file,
                    fmt::format("holds '{}', which is not a version", text));
    }

    std::variant<NoSelection, Selection, FileError>
    SelectVersion(const fs::path &directory,
                  const std::optional<fs::path> &home)
    {
        for (fs::path folder = directory;; folder = folder.parent_path())
        {
            const fs::path file = folder / fs::path(kProjectFileName);
            auto read = ReadVersionFile(file);
            if (auto *failure = std::get_if<FileError>(&read))
            {
                return std::move(*failure);
            }
            if (auto *version = std::get_if<Version>(&read))
            {
                return Selection{ *version, file, VersionSource::kProject };
            }
            if (folder == folder.parent_path())
            {
                break;
            }
        }
        if (!home)
        {
            return NoSelection{};
        }
        const fs::path file = ConfigFile(*home);
        auto read = ReadVersionFile(file);
        if (auto *failure = std::get_if<FileError>(&read))
        {
            return std::move(*failure);
        }
        if (auto *version = std::get_if<Version>(&read))
        {
            return Selection{ *version, file, VersionSource::kUser };
        }
        return NoSelection{};
    }

    std::optional<FileError>
    WriteVersionFile(const fs::path &file, const Version &version, bool force)
    {
        std::error_code error;
        const bool exists = fs::exists(fs::symlink_status(file, error));
        if (error && error != std::errc::no_such_file_or_directory)
        {
            return Fail(file, error.message());
        }
        if (exists && !force)
        {
            return Fail(file, "already exists; --force writes over it");
        }
        if (file.has_parent_path())
        {
            fs::create_directories(file.parent_path(), error);
            if (error)
            {
                return Fail(file.parent_path(), error.message());
            }
        }
        std::ofstream stream(file, std::ios::binary | std::ios::trunc);
        const std::string content = FormatVersion(version) + "\n";
        stream.write(content.data(),
                     static_cast<std::streamsize>(content.size()));
        stream.close();
        if (!stream)
        {
            return Fail(file, "cannot be written");
        }
        return std::nullopt;
    }

} // namespace Progmasoft::Tazelish
