// SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com>
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <Progmasoft/Catch3.hpp>
#include <cstdlib>
#include <fstream>
#include <string>

#include "Progmasoft/Tazelish/VersionFiles.hpp"

namespace fs = std::filesystem;
namespace tazelish = Progmasoft::Tazelish;

namespace
{

    // A folder of its own below the temporary folder of the test, emptied
    // first.
    fs::path
    Scratch(const std::string &name)
    {
        const char *root = std::getenv("TEST_TMPDIR"); // NOLINT
        const fs::path folder
            = (root != nullptr ? fs::path(root) : fs::temp_directory_path())
              / ("tazelish-" + name);
        fs::remove_all(folder);
        fs::create_directories(folder);
        return folder;
    }

    void
    WriteText(const fs::path &file, const std::string &text)
    {
        fs::create_directories(file.parent_path());
        std::ofstream(file, std::ios::binary) << text;
    }

    tazelish::Version
    Nightly()
    {
        return tazelish::Newest{ tazelish::Channel::kNightly };
    }

    std::string
    Selected(const fs::path &directory, const fs::path &home)
    {
        const auto selected = tazelish::SelectVersion(directory, home);
        REQUIRE(std::holds_alternative<tazelish::Selection>(selected));
        return tazelish::FormatVersion(
            std::get<tazelish::Selection>(selected).version);
    }

} // namespace

TEST_CASE("The nearest tazelish.txt comes before config.txt", "[files]")
{
    const fs::path root = Scratch("nearest");
    const fs::path home = root / "home";
    const fs::path project = root / "project";
    const fs::path nested = project / "Sources" / "Deep";
    fs::create_directories(nested);

    CHECK(std::holds_alternative<tazelish::NoSelection>(
        tazelish::SelectVersion(nested, home)));

    WriteText(tazelish::ConfigFile(home), "nightly\n");
    auto selected = tazelish::SelectVersion(nested, home);
    REQUIRE(std::holds_alternative<tazelish::Selection>(selected));
    CHECK(std::get<tazelish::Selection>(selected).source
          == tazelish::VersionSource::kUser);

    WriteText(project / "tazelish.txt", "\xEF\xBB\xBF 0.4.0.3\r\n");
    selected = tazelish::SelectVersion(nested, home);
    REQUIRE(std::holds_alternative<tazelish::Selection>(selected));
    const auto &selection = std::get<tazelish::Selection>(selected);
    CHECK(selection.source == tazelish::VersionSource::kProject);
    CHECK(selection.file == project / "tazelish.txt");
    CHECK(Selected(nested, home) == "0.4.0.3");

    WriteText(project / "Sources" / "tazelish.txt", "2026-11-05-unstable");
    CHECK(Selected(nested, home) == "2026-11-05-unstable");
}

TEST_CASE("A file that names no version is an error", "[files]")
{
    const fs::path root = Scratch("invalid");
    WriteText(root / "tazelish.txt", "latest\n");
    auto selected = tazelish::SelectVersion(root, std::nullopt);
    REQUIRE(std::holds_alternative<tazelish::FileError>(selected));
    CHECK(std::get<tazelish::FileError>(selected).message.find("'latest'")
          != std::string::npos);

    WriteText(root / "tazelish.txt", std::string(200, '1'));
    selected = tazelish::SelectVersion(root, std::nullopt);
    REQUIRE(std::holds_alternative<tazelish::FileError>(selected));
    CHECK(std::get<tazelish::FileError>(selected).message
          == "holds more than a version");

    fs::remove(root / "tazelish.txt");
    fs::create_directory(root / "tazelish.txt");
    CHECK(std::holds_alternative<tazelish::FileError>(
        tazelish::ReadVersionFile(root / "tazelish.txt")));
}

TEST_CASE("Writing over a version file needs --force", "[files]")
{
    const fs::path root = Scratch("force");
    const fs::path config = tazelish::ConfigFile(root / "home");

    CHECK_FALSE(tazelish::WriteVersionFile(config, Nightly(), false));
    auto read = tazelish::ReadVersionFile(config);
    REQUIRE(std::holds_alternative<tazelish::Version>(read));
    CHECK(std::get<tazelish::Version>(read) == Nightly());

    const tazelish::Version release = tazelish::Release{ 0, 4, 0, 3 };
    const auto refused = tazelish::WriteVersionFile(config, release, false);
    REQUIRE(refused.has_value());
    CHECK(refused->message.find("--force") != std::string::npos);
    CHECK(std::get<tazelish::Version>(tazelish::ReadVersionFile(config))
          == Nightly());

    CHECK_FALSE(tazelish::WriteVersionFile(config, release, true));
    CHECK(std::get<tazelish::Version>(tazelish::ReadVersionFile(config))
          == release);
}
