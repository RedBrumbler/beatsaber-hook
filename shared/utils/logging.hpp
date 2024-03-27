#pragma once

#include "paper/shared/logger.hpp"

namespace il2cpp_utils {
static constexpr const auto Logger = ::Paper::ConstLoggerContext("beatsaber-hook");
}

//nullable string
namespace bs_hooks {
struct nullable {
    const char* str;
    explicit constexpr nullable(const char* s) : str(s) {}
};

constexpr auto format_as(nullable n) {
    return n.str ? n.str : "(null)";
}
}  // namespace my
