// SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com>
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <cstdio>
#include <filesystem>
#include <fmt/format.h>
#include <string_view>
#include <vector>

#include "Progmasoft/Tazelish/CommandLine.hpp"
#include "Progmasoft/Tazelish/Help.hpp"
#include "Progmasoft/Tazelish/VersionFiles.hpp"

namespace
{

    namespace fs = std::filesystem;
    namespace tazelish = Progmasoft::Tazelish;

    constexpr int kSuccess = 0;
    constexpr int kFailure = 1;
    constexpr int kUsage = 2;

    int
    Report(const tazelish::FileError &error)
    {
        fmt::print(stderr,
                   "error: {} {}\n",
                   error.file.string(),
                   error.message);
        return kFailure;
    }

    int
    Write(const fs::path &file, const tazelish::Version &version, bool force)
    {
        if (auto error = tazelish::WriteVersionFile(file, version, force))
        {
            return Report(*error);
        }
        fmt::print("Wrote {} to {}\n",
                   tazelish::FormatVersion(version),
                   file.string());
        return kSuccess;
    }

    int
    RunUse(const tazelish::Use &use)
    {
        return Write(fs::path(".") / fs::path(tazelish::kProjectFileName),
                     use.version,
                     use.force);
    }

    int
    RunDefault(const tazelish::Default &choice)
    {
        const auto home = tazelish::HomeDirectory();
        if (!home)
        {
#ifdef _WIN32
            fmt::print(stderr, "error: USERPROFILE is not set\n");
#else
            fmt::print(stderr, "error: HOME is not set\n");
#endif
            return kFailure;
        }
        return Write(tazelish::ConfigFile(*home), choice.version, choice.force);
    }

    // Installing and updating download from static.tazel.net, which this build
    // of Tazelish does not do.
    int
    RunDownloading(std::string_view command)
    {
        fmt::print(
            stderr,
            "error: tazelish {} downloads from static.tazel.net, and this "
            "build of Tazelish does not download\n",
            command);
        return kFailure;
    }

    std::string_view
    SelfName(tazelish::SelfAction action)
    {
        switch (action)
        {
            case tazelish::SelfAction::kUpdate:
                return "self update";
            case tazelish::SelfAction::kRemove:
                return "self remove";
            case tazelish::SelfAction::kPurge:
                return "self purge";
        }
        return "self";
    }

    int
    Run(const tazelish::Command &command)
    {
        if (const auto *help = std::get_if<tazelish::Help>(&command))
        {
            fmt::print("{}", tazelish::HelpText(help->topic));
            return kSuccess;
        }
        if (const auto *use = std::get_if<tazelish::Use>(&command))
        {
            return RunUse(*use);
        }
        if (const auto *choice = std::get_if<tazelish::Default>(&command))
        {
            return RunDefault(*choice);
        }
        if (std::holds_alternative<tazelish::Install>(command))
        {
            return RunDownloading("install");
        }
        return RunDownloading(
            SelfName(std::get<tazelish::Self>(command).action));
    }

} // namespace

int
main(int argc, char **argv)
{
    std::vector<std::string_view> arguments;
    for (int index = 1; index < argc; ++index)
    {
        arguments.emplace_back(argv[index]);
    }
    const auto parsed = tazelish::ParseCommandLine(arguments);
    if (const auto *rejection = std::get_if<tazelish::Rejection>(&parsed))
    {
        fmt::print(stderr, "error: {}\n", rejection->message);
        return kUsage;
    }
    return Run(std::get<tazelish::Command>(parsed));
}
