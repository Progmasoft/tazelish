<!-- SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com> -->
<!-- SPDX-License-Identifier: MIT -->

# Development

Tazelish is C++17, built with Bazel through Bazelisk.

```sh
bazelisk build //:tazelish
bazelisk test //...
doxygen Doxyfile         # the API documentation, in doxygen/
```

The tests in `Tests/` cover the command line, versions and the version files. CI builds and tests on Linux, macOS
and Windows, collects coverage, and builds the Doxygen documentation.
