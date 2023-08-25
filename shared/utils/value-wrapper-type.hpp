#pragma once

namespace bs_hook {
    struct ValueTypeWrapper {
        constexpr ValueTypeWrapper() = default;
        ~ValueTypeWrapper() = default;

        constexpr ValueTypeWrapper(ValueTypeWrapper&&) = default;
        constexpr ValueTypeWrapper(ValueTypeWrapper const&) = default;

        constexpr ValueTypeWrapper& operator=(ValueTypeWrapper&&) = default;
        constexpr ValueTypeWrapper& operator=(ValueTypeWrapper const&) = default;
    };
}
