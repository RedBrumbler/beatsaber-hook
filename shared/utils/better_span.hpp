#pragma once

#include <initializer_list>
#include <span>
#include <type_traits>

template <typename T, class Ptr>
struct ArrayW;

template <typename T, class Ptr>
struct ListWrapper;

namespace bs_hook {

///
/// This version of span allows for better implicit conversions from
/// common types such as initializer list, vector, etc.
///
template <typename T, std::size_t Size = std::dynamic_extent>
struct better_span : public std::span<T, Size> {
    using base_type = std::span<T, Size>;

    template <std::size_t ArraySize>
        requires(ArraySize == Size)
    constexpr better_span(std::array<T, Size>& arr) : base_type(arr.begin(), arr.end()) {}
    template <std::size_t ArraySize>
        requires(ArraySize == Size)
    constexpr better_span(std::array<T, Size> const& arr) : base_type(arr.begin(), arr.end()) {}

    constexpr better_span(std::vector<T>& arr) : base_type(arr.begin(), arr.end()) {}
    constexpr better_span(std::vector<T> const& arr) : base_type(arr.cbegin(), arr.cend()) {}

    // initializer list only supports const
    constexpr better_span(std::initializer_list<T> const& arr)
        requires(std::is_const_v<T>)
        : base_type(arr.begin(), arr.end()) {}

    template <typename UPtr>
    constexpr better_span(ArrayW<T, UPtr> arr) : base_type(arr.begin(), arr.end()) {}

    template <typename UPtr>
    constexpr better_span(ListWrapper<T, UPtr> arr) : base_type(arr.begin(), arr.end()) {}

    // make it work like span
    template <typename... TArgs>
    inline better_span(TArgs&&... args) : base_type(std::forward<TArgs>(args)...) {}

    constexpr better_span() = default;
    constexpr better_span(better_span const&) = default;
    constexpr better_span(better_span&&) = default;

    constexpr better_span(base_type base) : base_type(base) {}

    constexpr better_span<T, Size>& operator=(better_span const&) = default;
    constexpr better_span<T, Size>& operator=(better_span&&) = default;
};
}  // namespace bs_hook