#pragma once
#include <optional>
#include <vector>
#include <span>
#include "il2cpp-type-check.hpp"
#include "base-wrapper-type.hpp"
#include "value-wrapper-type.hpp"
#include <stdexcept>

#if __has_include(<concepts>)
#include <concepts>
#elif __has_include(<experimental/concepts>)
#include <experimental/concepts>
#else
#warning "Please have some form of concepts support!"
#endif

template<class T, class U>
/// @brief If type T can be assigned to by type U&&
/// @tparam T The left hand side of the assignment
/// @tparam U The right hand side of the assignment
concept has_assignment = requires (T arg, U&& other) {
    arg = other;
};

template<class T>
requires (!std::is_reference_v<T>)
/// @brief A std::wrapper_reference implementation that perfect forwards to contained assignment operators.
struct WrapperRef {
    /// @brief Explicitly create a wrapper reference from a reference who MUST LIVE LONGER THAN THIS INSTANCE!
    explicit constexpr WrapperRef(T& instance) : ptr(std::addressof(instance)) {}
    // Assignment for any U type that is valid to be assigned.
    template<class U>
    requires (has_assignment<T, U>)
    WrapperRef& operator=(U&& other) {
        *ptr = other;
        return *this;
    }
    // Standard Assignment
    WrapperRef& operator=(const WrapperRef& x) noexcept = default;
    // Getter operation, implicit conversion
    constexpr operator T& () const noexcept {
        return *ptr;
    }
    // Getter operation, explicit conversion
    constexpr T& get() const noexcept {
        return *ptr;
    }
    // Invoke operation
    template<class... ArgTypes>
    constexpr std::invoke_result_t<T&, ArgTypes...> operator() (ArgTypes&&... args) const {
        return std::invoke(get(), std::forward<ArgTypes>(args)...);
    }
    private:
    T* ptr;
};

#pragma pack(push)

#include "il2cpp-object-internals.h"

#if IL2CPP_COMPILER_MSVC
#pragma warning( push )
#pragma warning( disable : 4200 )
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif

#include "utils.h"
#include "il2cpp-utils-methods.hpp"
#include <initializer_list>
#include "size-concepts.hpp"

/// @brief Represents an exception thrown from usage of an Array.
struct ArrayException : il2cpp_utils::exceptions::StackTraceException {
    void* arrayInstance;
    ArrayException(void* instance, std::string_view msg) : il2cpp_utils::exceptions::StackTraceException(msg.data()), arrayInstance(instance) {}
};

/// @brief An Array wrapper type that is responsible for holding an (ideally valid) pointer to an array on the GC heap.
/// Allows for C++ array semantics. Ex, [], begin(), end(), etc...
template<typename T>
struct ArrayW;

template<class T>
struct Array : public Il2CppArray
{
    ALIGN_TYPE(8) T values[IL2CPP_ZERO_LEN_ARRAY];

    inline il2cpp_array_size_t Length() const noexcept {
        if (bounds) {
            return bounds->length;
        }
        return max_length;
    }
    inline void assertBounds(size_t i) {
        if (i < 0 || i >= Length()) {
            throw ArrayException(this, string_format("%zu is out of bounds for array of length: %zu", i, Length()));
        }
    }
    T& operator[](size_t i) noexcept {
        return values[i];
    }
    const T& operator[](size_t i) const noexcept {
        return values[i];
    }

    /// @brief Get a given index, performs bound checking and throws std::runtime_error on failure.
    /// @param i The index to get.
    /// @return The reference to the item.
    T& get(size_t i) {
        assertBounds(i);
        return values[i];
    }
    /// @brief Get a given index, performs bound checking and throws std::runtime_error on failure.
    /// @param i The index to get.
    /// @return The const reference to the item.
    const T& get(size_t i) const {
        assertBounds(i);
        return values[i];
    }
    /// @brief Tries to get a given index, performs bound checking and returns a std::nullopt on failure.
    /// @param i The index to get.
    /// @return The WrapperRef<T> to the item, mostly considered to be a T&.
    std::optional<WrapperRef<T>> try_get(size_t i) noexcept {
        if (i >= Length() || i < 0) {
            return std::nullopt;
        }
        return WrapperRef(values[i]);
    }
    /// @brief Tries to get a given index, performs bound checking and returns a std::nullopt on failure.
    /// @param i The index to get.
    /// @return The WrapperRef<const T> to the item, mostly considered to be a const T&.
    std::optional<WrapperRef<const T>> try_get(size_t i) const noexcept {
        if (i >= Length() || i < 0) {
            return std::nullopt;
        }
        return WrapperRef(values[i]);
    }

    static Array<T>* New(std::initializer_list<T> vals) {
        il2cpp_functions::Init();
        auto* arr = reinterpret_cast<Array<T>*>(il2cpp_functions::array_new(
            il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<T>::get(), vals.size()));
        if (!arr) {
            throw ArrayException(nullptr, "Could not create Array!");
        }
        memcpy(arr->values, vals.begin(), sizeof(T)*vals.size());
        return arr;
    }

    static Array<T>* NewLength(il2cpp_array_size_t size) {
        il2cpp_functions::Init();
        auto arr = reinterpret_cast<Array<T>*>(il2cpp_functions::array_new(
            il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<T>::get(), size));
        if (!arr) {
            throw ArrayException(nullptr, "Could not create Array!");
        }
        return arr;
    }

    template<typename... TArgs>
    static Array<T>* New(TArgs&&... args) {
        return New({args...});
    }

    template<class U = Il2CppObject*>
    U GetEnumerator() {
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(
            this, "System.Collections.Generic.IEnumerable`1.GetEnumerator", 0));
        return il2cpp_utils::RunMethodRethrow<U, false>(this, method);
    }

    bool Contains(T item) {
        // TODO: find a better way around the existence of 2 methods with this name (the 2nd not being generic at all)
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(
            this, "System.Collections.Generic.ICollection`1.Contains", 1));
        return il2cpp_utils::RunMethodRethrow<bool, false>(this, method, item);
    }
    void CopyTo(::Array<T>* array, int arrayIndex) {
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(
            this, "System.Collections.Generic.ICollection`1.CopyTo", 2));
        il2cpp_utils::RunMethodRethrow<void, false>(this, method, array, arrayIndex);
    }
    int IndexOf(T item) {
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(this, "System.Collections.Generic.IList`1.IndexOf", 1));
        return il2cpp_utils::RunMethodRethrow<int, false>(this, method, item);
    }
    /// @brief Copies the array to the provided vector reference of same type.
    /// @param vec The vector to copy to.
    void copy_to(std::vector<T>& vec) const {
        vec.assign(values, values + Length());
    }
    /// @brief Provides a reference span of the held data within this array. The span should NOT outlive this instance.
    /// @return The created span.
    std::span<T> ref_to() {
        return std::span(values, Length());
    }
    /// @brief Provides a reference span of the held data within this array. The span should NOT outlive this instance.
    /// @return The created span.
    const std::span<T> ref_to() const {
        return std::span(const_cast<T*>(values), Length());
    }

    protected:
        friend ArrayW<T>;
        void* internal_get(std::size_t i) noexcept {
            return values + i;
        }

        const void* internal_get(std::size_t i) const {
            return values + i;
        }
};

template<typename TArg>
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<Array<TArg>*> {
    static inline Il2CppClass* get() {
        il2cpp_functions::Init();
        if constexpr (std::is_same_v<std::decay_t<TArg>, Il2CppObject*>) {
            il2cpp_functions::CheckS_GlobalMetadata();
            return il2cpp_functions::array_class_get(il2cpp_functions::defaults->object_class, 1);
        } else {
            static auto& logger = getLogger();
            Il2CppClass* eClass = RET_0_UNLESS(logger, il2cpp_no_arg_class<TArg>::get());
            return il2cpp_functions::array_class_get(eClass, 1);
        }
    }
};

/// il2cpp safe array, meaning whatever is stored in the array matches size between cpp & c#, so casting pointers is allowed
template<typename T>
requires(::il2cpp_utils::is_il2cpp_size_safe_v<T>)
struct ArrayW<T> {
    using Ptr = Array<T>*;
    /// @brief init ArrayW from arbitrary pointer
    constexpr explicit ArrayW(void* i) noexcept : val(static_cast<Ptr>(i)) {}
    /// @brief init ArrayW from pointer array
    constexpr ArrayW(Ptr initVal) noexcept : val(initVal) {}
    /// @brief explicit nullptr init
    constexpr ArrayW(std::nullptr_t nptr) noexcept : val(nptr) {}
    /// @brief default ctor begins as nullptr
    ArrayW() noexcept : val(nullptr) {}
    template<class U>
    requires (!std::is_same_v<std::nullptr_t, U> && std::is_convertible_v<U, T>)
    ArrayW(std::initializer_list<U> vals) : val(Array<T>::New(vals)) {}
    ArrayW(il2cpp_array_size_t size) : val(Array<T>::NewLength(size)) {}

    inline il2cpp_array_size_t Length() const {
        return val->Length();
    }

    inline il2cpp_array_size_t size() const {
        return val->Length();
    }

    inline void assertBounds(std::size_t i) const {
        val->assertBounds(i);
    }

    T& operator[](std::size_t i) { return internal_get(i); }
    const T& operator[](std::size_t i) const { return internal_get(i); }

    /// @brief Get a given index, performs bound checking.
    /// @throw ArrayException thrown when bounds assert fails.
    /// @param i The index to get.
    /// @return The reference to the item.
    T& get(size_t i) {
        assertBounds(i);
        return internal_get(i);
    }
    /// @brief Get a given index, performs bound checking.
    /// @throw ArrayException thrown when bounds assert fails.
    /// @param i The index to get.
    /// @return The const reference to the item.
    const T& get(size_t i) const {
        assertBounds(i);
        return internal_get(i);
    }
    /// @brief Tries to get a given index, performs bound checking and returns a std::nullopt on failure.
    /// @param i The index to get.
    /// @return The WrapperRef<T> to the item, mostly considered to be a T&.
    std::optional<WrapperRef<T>> try_get(size_t i) noexcept {
        if (i >= Length() || i < 0) {
            return std::nullopt;
        }
        return WrapperRef(val->values[i]);
    }
    /// @brief Tries to get a given index, performs bound checking and returns a std::nullopt on failure.
    /// @param i The index to get.
    /// @return The WrapperRef<const T> to the item, mostly considered to be a const T&.
    std::optional<WrapperRef<const T>> try_get(size_t i) const noexcept {
        if (i >= Length() || i < 0) {
            return std::nullopt;
        }
        return WrapperRef(val->values[i]);
    }
    template<class U = Il2CppObject*>
    U GetEnumerator() {
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(
            val, "System.Collections.Generic.IEnumerable`1.GetEnumerator", 0));
        return il2cpp_utils::RunMethodRethrow<U, false>(val, method);
    }
    bool Contains(T item) {
        // TODO: find a better way around the existence of 2 methods with this name (the 2nd not being generic at all)
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(
            val, "System.Collections.Generic.ICollection`1.Contains", 1));
        return il2cpp_utils::RunMethodRethrow<bool, false>(val, method, item);
    }
    void CopyTo(::Array<T>* array, int arrayIndex) {
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(
            val, "System.Collections.Generic.ICollection`1.CopyTo", 2));
        il2cpp_utils::RunMethodRethrow<void, false>(val, method, array, arrayIndex);
    }
    int IndexOf(T item) {
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(val, "System.Collections.Generic.IList`1.IndexOf", 1));
        return il2cpp_utils::RunMethodRethrow<int, false>(val, method, item);
    }

    std::span<T> ref_to() {
        return std::span(static_cast<T*>(val->values), Length());
    }
    std::span<const T> ref_to() const {
        return std::span(static_cast<const T*>(val->values), Length());
    }
    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() noexcept { return val->values; }
    iterator end() noexcept { return val->values + Length(); }
    auto rbegin() { return std::reverse_iterator(val->values + Length()); }
    auto rend() { return std::reverse_iterator(val->values); }

    const_iterator begin() const noexcept { return val->values; }
    const_iterator end() const noexcept { return val->values + Length(); }
    auto rbegin() const { return std::reverse_iterator(val->values + Length()); }
    auto rend() const { return std::reverse_iterator(val->values); }

    explicit operator Ptr() noexcept { return val; }
    explicit operator const Ptr() const noexcept { return val; }
    explicit operator Il2CppArray*() noexcept { return val; }
    explicit operator const Il2CppArray*() const noexcept { return val; }
    constexpr Ptr operator->() noexcept { return val; }
    constexpr const Ptr operator->() const noexcept { return val; }

    template<class U>
    requires (il2cpp_sizeof(T) == il2cpp_sizeof(U) && std::is_convertible_v<T, U>)
    operator ArrayW<U>() noexcept {
        return ArrayW<U>(reinterpret_cast<Array<U>*>(val));
    }
    template<class U>
    requires (il2cpp_sizeof(T) == il2cpp_sizeof(U) && std::is_convertible_v<T, U>)
    operator ArrayW<U> const() const noexcept {
        return ArrayW<U>(reinterpret_cast<Array<U>* const>(val));
    }
    operator bool() const noexcept { return val != nullptr; }

    static ArrayW<T> Empty() noexcept { return ArrayW<T>(il2cpp_array_size_t(0)); };

    /// @brief attempts to get the first element in the array
    /// @throws ArrayException when the array is empty
    T First() {
        if (Length() > 0)
            return internal_get(0);
        else
            throw ArrayException(this, "ArrayW<T>.First() called on empty array");
    }

    /// @brief attempts to get the first element in the array, returns default constructed element if not found
    template<typename D = T>
    requires(std::is_default_constructible_v<T>)
    T FirstOrDefault() {
        if (Length() > 0)
            return internal_get(0);
        else
            return {};
    }

    /// @brief attempts to get the first element that returns true for the given predicate
    /// @throws ArrayException when the array is empty
    template<class Pred>
    T First(Pred predicate) {
        for (auto itr = begin(); itr != end(); itr++) {
            if (predicate(*itr)) return *itr;
        }
        throw ArrayException(this, "ArrayW<T>.First(predicate) could find no item that matched predicate");
    }

    /// @brief attempts to get the first element that returns true for the given predicate, returns default constructed element if not found
    template<class Pred>
    requires(std::is_default_constructible_v<T>)
    T FirstOrDefault(Pred predicate) {
        for (auto itr = begin(); itr != end(); itr++) {
            if (predicate(*itr)) return *itr;
        }
        return {};
    }

    /// @brief attempts to get the last element in the array
    /// @throws ArrayException when the array is empty
    T Last() {
        if (Length() > 0)
            return internal_get(Length() - 1);
        else
            throw ArrayException(this, "ArrayW<T>.Last() called on empty array");
    }

    /// @brief attempts to get the last element in the array, returns default constructed element if not found
    template<typename>
    requires(std::is_default_constructible_v<T>)
    T LastOrDefault() {
        if (Length() > 0)
            return internal_get(Length() - 1);
        else
            return {};
    }

    /// @brief attempts to get the last element in the array that matches the given predicate
    /// @throws ArrayException when the array is empty
    template<class Pred>
    T Last(Pred predicate) {
        for (auto itr = rbegin(); itr != rend(); itr++) {
            if (predicate(*itr)) return *itr;
        }
        throw ArrayException(this, "ArrayW<T>.Last(predicate) could find no item that matched predicate");
    }

    /// @brief attempts to get the last element in the array that matches the given predicate, returns default constructed element if not found
    template<class Pred>
    requires(std::is_default_constructible_v<T>)
    T LastOrDefault(Pred predicate) {
        for (auto itr = rbegin(); itr != rend(); itr++) {
            if (predicate(*itr)) return *itr;
        }
        return {};
    }

    void* convert() const noexcept { return const_cast<void*>(static_cast<void*>(val)); }
    protected:
        Ptr val;

        T& internal_get(std::size_t i) noexcept {
            return *static_cast<T*>(val->internal_get(i));
        }

        const T& internal_get(std::size_t i) const noexcept {
            return *static_cast<const T*>(val->internal_get(i));
        }
};

// TODO: implementation for !il2cpp_safe types, meaning conversion between what actually is stored and returns.

static_assert(il2cpp_utils::has_il2cpp_conversion<ArrayW<int*>>);
template<class T>
struct ::il2cpp_utils::il2cpp_type_check::need_box<ArrayW<T>> {
    constexpr static bool value = false;
};

template<class T>
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<ArrayW<T>> {
    static inline Il2CppClass* get() {
        static auto klass = ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<Array<T>*>::get();
        return klass;
    }
};

#pragma pack(pop)
