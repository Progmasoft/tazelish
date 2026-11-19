<!-- SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com> -->
<!-- SPDX-License-Identifier: MIT -->

# Architecture

| File | What it does |
| --- | --- |
| `Sources/Main.cpp` | The entry point: runs the command the command line names. |
| `Sources/CommandLine.cpp` | The command line, read by hand into a typed command: `install`, `use`, `default`, `self`, `help`. |
| `Sources/Version.cpp` | Reading and ordering versions, and the named channels such as `unstable` and `nightly`. |
| `Sources/VersionFiles.cpp` | `tazelish.txt` (the nearest, in this folder or above) and the user's `config.txt`, read and written; `--force` to write over one. |
| `Sources/Help.cpp` | The help of each command. |

The public headers are in `Headers/Progmasoft/Tazelish/`, one per source. `Build/` holds Bazel's toolchain settings.
