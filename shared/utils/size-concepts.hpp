#pragma once

#include "utils/base-wrapper-type.hpp"
#include "utils/value-wrapper-type.hpp"

namespace il2cpp_utils {
    template<typename T>
    concept reference_wrapper = std::is_base_of_v<::bs_hook::Il2CppWrapperType, T>;

    template<typename T>
    concept value_wrapper = std::is_base_of_v<::bs_hook::ValueTypeWrapper<T::VALUE_TYPE_SIZE>, T>;

    template<typename T>
    struct il2cpp_size {
        static constexpr auto value = sizeof(T);
    };

    template<value_wrapper T>
    struct il2cpp_size<T> {
        static constexpr auto value = T::VALUE_TYPE_SIZE;
    };

    template<reference_wrapper T>
    struct il2cpp_size<T> {
        static constexpr auto value = sizeof(void*);
    };

    template<typename T>
    static constexpr auto il2cpp_size_v = il2cpp_size<T>::value;


    template<typename T>
    struct is_il2cpp_size_safe {
        static constexpr bool value = il2cpp_size_v<T> == sizeof(T);
    };

    template<typename T>
    static constexpr bool is_il2cpp_size_safe_v = is_il2cpp_size_safe<T>::value;
}

#define il2cpp_sizeof(...) (::il2cpp_utils::il2cpp_size<__VA_ARGS__>::value)
#define il2cpp_safe(...) (::il2cpp_utils::is_il2cpp_size_safe_v<__VA_ARGS__>)
