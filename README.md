<!-- SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com> -->
<!-- SPDX-License-Identifier: MIT -->

# Tazelish

Tazelish installs [Tazel](https://github.com/Progmasoft/tazel) and chooses which version of it runs. It keeps
several versions side by side, for one user or for the whole machine, and runs the one a project or the user asks
for when you type `tazel`.

Tazelish is at its start: this repository holds no code yet. What it will do is described in
[docs/](docs/README.md). Tazel does not need it: Tazel installs and runs on its own, and without Tazelish you install,
update and remove its versions yourself.

## What it will do

- `tazelish install <version>`, `update`, `remove` and `list` manage the Tazel versions of the user;
  `tazelish machine ...` does the same for every user of the machine.
- `<version>` is an exact version (`1.4.0`, `1.5.0-rc.1`) or a selector: `stable`, `rolling`, `lts`, `elts`,
  `alpha`, `beta` or `rc`.
- `tazelish use <version>` writes `tazelish.txt` in the current directory and `tazelish default <version>` the
  user's default; `tazel` runs the version the nearest `tazelish.txt` names, else the default.
- `tazel` is Tazelish itself under a second name. It starts the chosen Tazel with the same arguments, directory,
  environment and streams, and returns its exit status; it never downloads or installs anything.
- Every package is checked against signed metadata (Ed25519) and its SHA-256 before it is installed.
- Linux (glibc and musl) and macOS on x64 and ARM64, and Windows on x64 and ARM64.

## License

MIT ([LICENSE.txt](LICENSE.txt)), with the Progmasoft Patent Grant ([PATENTS.md](PATENTS.md)).
