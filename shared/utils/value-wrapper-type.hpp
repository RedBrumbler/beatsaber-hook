#pragma once
#include <cstddef>
#include <array>

namespace bs_hook {
    template<std::size_t sz>
    struct ValueTypeWrapper {
        static constexpr auto VALUE_TYPE_SIZE = sz;

        constexpr explicit ValueTypeWrapper(std::array<std::byte, VALUE_TYPE_SIZE> i) noexcept : instance(std::move(i)) {}
        void* convert() const noexcept { return const_cast<void*>(static_cast<const void*>(instance.data())); }

        constexpr ValueTypeWrapper() = default;
        ~ValueTypeWrapper() = default;

        constexpr ValueTypeWrapper(ValueTypeWrapper&&) = default;
        constexpr ValueTypeWrapper& operator=(ValueTypeWrapper&& o) {
            instance = std::move(o.instance);
            return *this;
        }

        constexpr ValueTypeWrapper(ValueTypeWrapper const&) = default;
        constexpr ValueTypeWrapper& operator=(ValueTypeWrapper const& o) {
            instance = o.instance;
            return *this;
        }

        std::array<std::byte, sz> instance;
    };
}
