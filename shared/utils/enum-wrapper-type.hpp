#pragma once

#include "value-wrapper-type.hpp"

namespace bs_hook {
    struct EnumTypeWrapper : public ValueTypeWrapper {
        constexpr EnumTypeWrapper() = default;
        ~EnumTypeWrapper() = default;

        constexpr EnumTypeWrapper(EnumTypeWrapper&&) = default;
        constexpr EnumTypeWrapper(EnumTypeWrapper const&) = default;

        constexpr EnumTypeWrapper& operator=(EnumTypeWrapper&&) = default;
        constexpr EnumTypeWrapper& operator=(EnumTypeWrapper const&) = default;
    };
}
