<!-- SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com> -->
<!-- SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0 -->

# Tazelish

Tazelish installs [Tazel](https://tazel.net/) and chooses which version of it runs. It is for those who use Tazel
alone; the toolchain manager of the whole of Tala is Talup. Tazelish runs on Linux (glibc 2.39 or newer, or musl 1.2.4
or newer), macOS 14 or newer and Windows with PowerShell 5.1; it does not run on BSD.

```text
tazelish install 0.4.0.3              a release: MAJOR.SEMIMAJOR.MINOR.PATCH
tazelish install unstable             the newest unstable build
tazelish install nightly              the newest nightly build
tazelish install 2026-11-05-nightly   the nightly build of a day
tazelish use [--force] 0.4.0.3        writes the version to ./tazelish.txt
tazelish default [--force] nightly    writes the version to the config.txt of the user
tazelish self update|remove|purge
tazelish help [command]
```

`tazelish.txt` and `config.txt` hold one version each, written as it is written after `tazelish install`. The nearest
`tazelish.txt`, in the current folder or a folder above it, comes first; `config.txt`, at `$HOME/.tazelish/config.txt`
or `%USERPROFILE%\.tazelish\config.txt`, counts where none names a version. `use` and `default` refuse to write over a
file that exists unless `--force` is given.

This build reads the command line, the versions and both files. `install` and `self` download from
`static.tazel.net`, which this build does not do; they end with exit code 1.

## Building

Tazelish is C++17, built with Bazel through Bazelisk:

```bash
bazelisk test //...
bazelisk build //:tazelish
```

On Windows it is compiled by clang-cl: set `BAZEL_LLVM` to the folder of an LLVM installation first, for example
`C:\Program Files\LLVM`. The code of Tazelish throws no exceptions and builds with warnings as errors.

| Folder | Holds |
| --- | --- |
| `Headers/Progmasoft/Tazelish/` | the interfaces: versions, the command line, the version files, the help |
| `Sources/` | their implementation and `Main.cpp` |
| `Tests/` | the tests, written with [Catch3](https://github.com/Progmasoft/catch3) |
| `Build/` | a patch of rules_cc for clang-cl |

## Licence

MPL-2.0 with the Progmasoft Exception 2.0; see [LICENSE.txt](LICENSE.txt) and
[LICENSES/](LICENSES/AdditionRef-Progmasoft-Exception-2.0.txt).
