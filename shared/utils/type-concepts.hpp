#pragma once

#include <array>
#include <type_traits>

namespace il2cpp_utils {
    template<typename T, typename U>
    concept convertible_to = std::is_convertible_v<T, U>;

    template<class T>
    /// @brief A concept depicting if a type is a wrapper type.
    // TODO: Make this use a static creation method instead of a constructor
    concept has_il2cpp_conversion = requires (T t) {
        {t.convert()} -> std::same_as<void*>;
        std::is_constructible_v<T, void*>;
    };

#pragma region value marker
    /// @brief not all types will have the value marker
    template<typename T>
    concept has_value_marker = requires {
        { T::__IL2CPP_VALUE_TYPE } -> convertible_to<bool>;
    };

    /// @brief all types will evaluate false for the check
    template<typename T, bool check>
    struct value_marker_check {
        constexpr static bool value = false;
    };

    /// @brief types that have the marker will actually evaluate the check
    template<has_value_marker T, bool check>
    struct value_marker_check<T, check> {
        constexpr static bool value = T::__IL2CPP_VALUE_TYPE == check;
    };

    template<has_value_marker T, bool check>
    constexpr bool value_marker_check_v = value_marker_check<T, check>::value;

#pragma endregion // value marker

#pragma region val type
    template <typename T>
    concept il2cpp_value_type_requirements = requires(T const& t) {
        requires(std::is_same_v<decltype(t.instance), std::array<std::byte, sizeof(t.instance)>>);
        requires(std::is_constructible_v<T, decltype(t.instance)>);
        requires(il2cpp_utils::value_marker_check_v<T, true>);
    };

    /// @brief mark a T explicitly as value type, default is false
    template<class T> struct ValueTypeTrait {
        constexpr static bool value = false;
    };

    /// @brief anything that matches the type requirements should also be allowed to be a value type
    template <il2cpp_value_type_requirements T> struct ValueTypeTrait<T> {
        constexpr static bool value = true;
    };

    /// @brief mark a generic T explicitly as value type without requiring its TArgs to be fully realized, defaults to false
    template<template<class...> class T> struct GenValueTypeTrait {
        constexpr static bool value = false;
    };

    /// @brief non generics should return false
    template<class T> struct ValueDecompose {
        constexpr static bool value = false;
    };

    /// @brief generics should fall back to the generic type trait
    template<template<class...> class T, class... TArgs> struct ValueDecompose<T<TArgs...>> {
        constexpr static bool value = GenValueTypeTrait<T>::value;
    };

    /// @brief concept that lets us know whether what we are dealing with is a value type or not
    template<class T>
    concept il2cpp_value_type = ValueDecompose<T>::value || ValueTypeTrait<T>::value;
#pragma endregion // val type

#pragma region ref type
    // https://godbolt.org/z/4vveWa46Y
    // Standard ref type concept
    // Note that this requires that type T is full instantiated
    // We want to ALSO support a case where that's not the case
    template <typename T>
    concept il2cpp_reference_type_requirements = requires(T const& t) {
        { t.convert() } -> convertible_to<void*>;

        // ensure these constructors exist
        requires std::is_constructible_v<T, void*>;
        requires std::is_constructible_v<T, std::nullptr_t>;

        requires(::il2cpp_utils::value_marker_check_v<T, false>);
    };

    /// @brief mark a T explicitly as reference type, default is false
    template <class T> struct RefTypeTrait;

    /// @brief anything that matches the type requirements should also be allowed to be a reference type
    template <il2cpp_reference_type_requirements T> struct RefTypeTrait<T> {
        constexpr static bool value = true;
    };

    /// @brief mark a generic T explicitly as reference type without requiring its TArgs to be fully realized, defaults to false
    template<template<class...> class T> struct GenRefTypeTrait;

    /// @brief non generics should return false
    template<class T> struct RefDecompose {
        constexpr static bool value = false;
    };

    /// @brief generics should fall back to the generic type trait
    template<template<class...> class T, class... TArgs> struct RefDecompose<T<TArgs...>> {
        constexpr static bool value = GenRefTypeTrait<T>::value;
    };

    /// @brief concept that lets us know whether what we are dealing with is a reference type or not
    template <class T>
    concept il2cpp_reference_type = RefDecompose<T>::value || RefTypeTrait<T>::value;
#pragma endregion // ref type

#define MARK_REF_T(...) \
    template<> struct ::il2cpp_utils::RefTypeTrait<__VA_ARGS__> { static constexpr bool value = true; }; \
    template<> struct ::il2cpp_utils::ValueTypeTrait<__VA_ARGS__> { static constexpr bool value = false; }

#define MARK_VAL_T(...) \
    template<> struct ::il2cpp_utils::RefTypeTrait<__VA_ARGS__> { static constexpr bool value = false; }; \
    template<> struct ::il2cpp_utils::ValueTypeTrait<__VA_ARGS__> { static constexpr bool value = true; }

#define MARK_GEN_REF_T(...) \
    template<> struct ::il2cpp_utils::GenRefTypeTrait<__VA_ARGS__> { static constexpr bool value = true; }; \
    template<> struct ::il2cpp_utils::GenValueTypeTrait<__VA_ARGS__> { static constexpr bool value = false; }

#define MARK_GEN_VAL_T(...) \
    template<> struct ::il2cpp_utils::GenRefTypeTrait<__VA_ARGS__> { static constexpr bool value = false; }; \
    template<> struct ::il2cpp_utils::GenValueTypeTrait<__VA_ARGS__> { static constexpr bool value = true; }

}
