#pragma once
// This file should only ever be included in typedefs.h

#include <span>
#include <optional>
#include "typedefs-array.hpp"
#include "size-concepts.hpp"
#include "typedefs-object.hpp"

// System.Collections.Generic.List
template<class T>
struct List : Il2CppObject
{
    ArrayW<T> items;
    int size;
    int version;
    Il2CppObject* syncRoot;
};
DEFINE_IL2CPP_ARG_TYPE_GENERIC_CLASS(List, "System.Collections.Generic", "List`1");

template<class T, class Ptr = List<T>*>
struct ListW;

MARK_GEN_REF_T(ListW);

// il2cpp safe implementation of ListW
template<class T, class Ptr>
struct ListW : public ::ObjectWrapperType {
    static_assert(sizeof(Ptr) == sizeof(void*), "Size of Ptr type must be the same as a void*!");

    // TODO: Consider requirementally constexpr-ifying this call
    // TODO: Apply these il2cpp conversion changes to ArrayW as well, to permit ArrayW to hold wrapper types and not pure pointers
    constexpr ListW(Ptr const& p) noexcept : ::ObjectWrapperType(nullptr) { set_ptr(p); }

    template<class V = void>
    requires (std::is_pointer_v<Ptr> && !il2cpp_utils::has_il2cpp_conversion<Ptr>)
    explicit constexpr ListW(void* alterInit) noexcept : ::ObjectWrapperType(alterInit) {}

    constexpr ListW(std::span<T> const p) : ::ObjectWrapperType(il2cpp_utils::NewSpecific<Ptr>(p.size())) {
        std::copy(p.begin(), p.end(), begin());
    }

    [[nodiscard]] constexpr int size() const {
        return get_ptr()->size;
    }
    decltype(auto) operator[](size_t i) {
        return get_ptr()->items[i];
    }
    decltype(auto) operator[](size_t i) const {
        return get_ptr()->items[i];
    }

    /// @brief Get a given index, performs bound checking and throws std::runtime_error on failure.
    /// @param i The index to get.
    /// @return The reference to the item.
    decltype(auto) get(size_t i) {
        THROW_UNLESS(i < size() && i >= 0);
        return get_ptr()->items[i];
    }
    /// @brief Get a given index, performs bound checking and throws std::runtime_error on failure.
    /// @param i The index to get.
    /// @return The const reference to the item.
    decltype(auto) get(size_t i) const {
        THROW_UNLESS(i < size() && i >= 0);
        return get_ptr()->items[i];
    }
    /// @brief Tries to get a given index, performs bound checking and returns a std::nullopt on failure.
    /// @param i The index to get.
    /// @return The WrapperRef<T> to the item, mostly considered to be a T&.
    decltype(auto) try_get(size_t i) {
        if (i >= size() || i < 0) {
            return std::nullopt;
        }
        return WrapperRef(get_ptr()->items[i]);
    }
    /// @brief Tries to get a given index, performs bound checking and returns a std::nullopt on failure.
    /// @param i The index to get.
    /// @return The WrapperRef<const T> to the item, mostly considered to be a const T&.
    decltype(auto) try_get(size_t i) const {
        if (i >= size() || i < 0) {
            return std::nullopt;
        }
        return WrapperRef(get_ptr()->items[i]);
    }

    decltype(auto) begin() {
        return get_ptr()->items.begin();
    }
    decltype(auto) end() {
        return get_ptr()->items.begin() + size();
    }
    decltype(auto) begin() const {
        return get_ptr()->items.begin();
    }
    decltype(auto) end() const {
        return get_ptr()->items.begin() + size();
    }

    template<typename D = T>
    requires(std::is_same_v<D, T> && il2cpp_utils::is_il2cpp_size_safe_v<D>)
    operator std::span<D const> () const {
        return std::span<D const>(this->begin(), this->end());
    }

    template<typename D = T>
    requires(std::is_same_v<D, T> && il2cpp_utils::is_il2cpp_size_safe_v<D>)
    operator std::span<D> () {
        return std::span<D>(this->begin(), this->end());
    }

    operator Ptr() noexcept {
        return get_ptr();
    };

    Ptr operator ->() noexcept { return get_ptr(); }
    Ptr const operator ->() const noexcept { return get_ptr(); }

    private:
        constexpr Ptr get_ptr() noexcept { return static_cast<Ptr>(instance); }
        constexpr const Ptr get_ptr() const noexcept { return static_cast<const Ptr>(instance); }
        constexpr void set_ptr(Ptr p) noexcept { instance = static_cast<void*>(p); }
};

static_assert(sizeof(ListW<int>) == sizeof(void*));
static_assert(il2cpp_utils::is_il2cpp_size_safe_v<int>);
static_assert(il2cpp_utils::is_il2cpp_size_safe_v<List<int>*>);
static_assert(il2cpp_utils::has_il2cpp_conversion<ListW<int, List<int>*>>);

template<class T, class Ptr>
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<ListW<T, Ptr>> {
    static inline Il2CppClass* get() {
        static auto klass = ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<Ptr>::get();
        return klass;
    }
};

template<class T, class Ptr>
struct ::il2cpp_utils::il2cpp_type_check::need_box<ListW<T, Ptr>> {
    constexpr static bool value = false;
};
