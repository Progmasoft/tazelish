// SPDX-FileCopyrightText: 2026 Progmasoft <support@progmasoft.com>
// SPDX-License-Identifier: MIT

/// \file
/// The text `tazelish help` prints.

#pragma once

#include <optional>
#include <string_view>

#include "Progmasoft/Tazelish/CommandLine.hpp"

namespace Progmasoft::Tazelish
{

    /// The general help without a topic, or the help of one command.
    /// @param topic The command to explain, or nothing.
    /// @return The text to print, ending with a newline.
    [[nodiscard]] std::string_view
    HelpText(std::optional<Topic> topic);

} // namespace Progmasoft::Tazelish
