#pragma once

#include "value-wrapper-type.hpp"

namespace bs_hook {
    // 0 special case, but otherwise the size should be an actual amount of bytes it could be
    template<std::size_t sz>
    requires(sz == 0x0 || sz == 0x1 || sz == 0x2 || sz == 0x4 || sz == 0x8)
    struct EnumTypeWrapper : public ValueTypeWrapper<sz> {
        static constexpr auto VALUE_TYPE_SIZE = ValueTypeWrapper<sz>::VALUE_TYPE_SIZE;
        using ValueTypeWrapper<sz>::ValueTypeWrapper;

        constexpr EnumTypeWrapper() = default;
        ~EnumTypeWrapper() = default;

        constexpr EnumTypeWrapper(EnumTypeWrapper&&) = default;
        constexpr EnumTypeWrapper(EnumTypeWrapper const&) = default;

        constexpr EnumTypeWrapper& operator=(EnumTypeWrapper&&) = default;
        constexpr EnumTypeWrapper& operator=(EnumTypeWrapper const&) = default;
    };

    /// @brief struct to pass a pointer to an enum into a method
    struct EnumPtr : public VTPtr {
        template<std::size_t sz>
        EnumPtr(EnumTypeWrapper<sz>& e) : VTPtr(e) {};

        explicit EnumPtr(void* i) : VTPtr(i) {};
    };
}

template<std::size_t sz>
struct ::il2cpp_utils::ValueTypeTrait<::bs_hook::EnumTypeWrapper<sz>> {
    constexpr static bool value = true;
};
