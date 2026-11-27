<!-- SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com> -->
<!-- SPDX-License-Identifier: MIT -->

# Design of Tazelish

This is what Tazelish is built to do. Nothing here is implemented yet; as parts are, each gets its own page here
and this one points to it.

## Two names, one program

Tazelish is one executable published under two names in the directory of its scope, as a hard link (a symbolic link
where a hard link cannot be made; never a copy):

| Name | Does |
| --- | --- |
| `tazelish` | manages Tazel versions and Tazelish itself; it takes only its own commands |
| `tazel` | starts the chosen Tazel, passing everything on unchanged |

## Scopes and files

| | Linux and macOS | Windows |
| --- | --- | --- |
| User state | `$HOME/.tazelish/` | `%USERPROFILE%\.tazelish\` |
| Machine state | `/opt/Progmasoft/tazelish/` | `%ProgramData%\Progmasoft\Tazelish\` |
| User commands | `$HOME/.local/bin/` | `%APPDATA%\Progmasoft\Tazelish\` |
| Machine commands | `/usr/local/bin/` | `%ProgramFiles%\Progmasoft\Tazelish\` |

A state directory holds `versions/` (the installed Tazel and Tazelish versions), `downloads/` (a cache of packages
named by their SHA-256), `tmp/`, `update-hash/` and `selectors.json` (the version each selector chose when it was last
installed or updated); the user's also holds `config.txt`, the default selection.

## Choosing the version

`tazel` reads the first of: `tazelish.txt` in the current directory, `tazelish.txt` in the nearest parent directory,
the user's `config.txt`. Each holds one version or selector. A selector is resolved from `selectors.json`, never from
the network; a version that is not installed is reported, not installed. User installations come before machine
ones.

## Versions and selectors

Versions follow SemVer 2.0.0: `MAJOR.MINOR.PATCH`, with `-alpha.N`, `-beta.N` or `-rc.N` for prereleases.

| Selector | Chooses |
| --- | --- |
| `rolling`, `lts`, `elts` | the newest release of that kind |
| `stable` | the newest of the three |
| `alpha`, `beta`, `rc` | the newest prerelease of that type |

A stable selector never chooses a prerelease or a release past its end of life; an exact version may, with a warning.

## Where packages come from

`https://storage.tazel.net/` serves a signed index and immutable version directories for Tazel and for Tazelish.
Tazelish checks the index's Ed25519 signature with the key built into it, then the version's manifest, then the
package's size and SHA-256, and installs nothing that fails a check. The first installation is done by a script from
`https://sh.tazel.net/tazelish/` (POSIX sh, PowerShell 7, or CMD), which checks the same signatures; Tazelish does
every later update and removal itself.

## Output

Every command takes `--message-format rich-stdio|script-friendly-stdio|jsonl`, `--color`, `--progress`, `-q` and
`-V`. Errors carry a code `E0000`–`E9999` that `tazelish explain` explains offline. Exit status 0 is success, 1 a
failure, 2 a command line Tazelish cannot take; `tazel` returns Tazel's own status.

## Building

C++17 with Bazel; tests with Progmasoft Catch3, benchmarks with ankerl::nanobench.
