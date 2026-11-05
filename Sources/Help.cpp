// SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com>
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include "Progmasoft/Tazelish/Help.hpp"

namespace Progmasoft::Tazelish
{
    namespace
    {

        constexpr std::string_view kGeneral
            = R"(Tazelish installs Tazel and chooses which version of it runs.

Usage: tazelish <command> [arguments]

Commands:
  install <version>          installs a version of Tazel
  use [--force] <version>    writes the version to ./tazelish.txt
  default [--force] <version>
                             writes the version to the config.txt of the user
  self update                updates Tazelish
  self remove                removes Tazelish and the versions it installed
  self purge                 also removes its settings, downloads and logs
  help [command]             prints this help, or the help of one command

A version is MAJOR.SEMIMAJOR.MINOR.PATCH, unstable, nightly,
YYYY-MM-DD-unstable or YYYY-MM-DD-nightly.

Exit codes: 0 success, 1 the work failed, 2 the command line was wrong.
)";

        constexpr std::string_view kInstall
            = R"(Usage: tazelish install <version>

Installs a version of Tazel:
  tazelish install 0.4.0.3               a release
  tazelish install unstable              the newest unstable build
  tazelish install nightly               the newest nightly build
  tazelish install 2026-11-05-unstable   the unstable build of a day
  tazelish install 2026-11-05-nightly    the nightly build of a day
)";

        constexpr std::string_view kUse
            = R"(Usage: tazelish use [--force] <version>

Writes the version to tazelish.txt in the current folder, so Tazel runs that
version there and in the folders below. The nearest tazelish.txt comes before
the config.txt of the user.

An existing tazelish.txt is an error; --force writes over it.
)";

        constexpr std::string_view kDefault
            = R"(Usage: tazelish default [--force] <version>

Writes the version to the config.txt of the user, the version Tazel runs where
no tazelish.txt names one:
  $HOME/.tazelish/config.txt             on Linux and macOS
  %USERPROFILE%\.tazelish\config.txt     on Windows

An existing config.txt is an error; --force writes over it.
)";

        constexpr std::string_view kSelf
            = R"(Usage: tazelish self update|remove|purge

  update   updates Tazelish
  remove   removes Tazelish, the versions of Tazel it installed and the PATH
           entries it added
  purge    does what remove does and also deletes the config.txt, the
           downloads and the logs of Tazelish
)";

        constexpr std::string_view kHelp = R"(Usage: tazelish help [command]

Prints the help of Tazelish, or the help of one command: install, use,
default, self or help.
)";

    } // namespace

    std::string_view
    HelpText(std::optional<Topic> topic)
    {
        if (!topic)
        {
            return kGeneral;
        }
        switch (*topic)
        {
            case Topic::kInstall:
                return kInstall;
            case Topic::kUse:
                return kUse;
            case Topic::kDefault:
                return kDefault;
            case Topic::kSelf:
                return kSelf;
            case Topic::kHelp:
                return kHelp;
        }
        return kGeneral;
    }

} // namespace Progmasoft::Tazelish
