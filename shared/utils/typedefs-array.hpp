#pragma once
#include <optional>
#include <vector>
#include <span>
#include "il2cpp-type-check.hpp"
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

/// @brief Represents an exception thrown from usage of an Array.
struct ArrayException : il2cpp_utils::exceptions::StackTraceException {
    void* arrayInstance;
    ArrayException(void* instance, std::string_view msg) : il2cpp_utils::exceptions::StackTraceException(msg.data()), arrayInstance(instance) {}
};

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

template<typename T, class Ptr = Array<T>*>
/// @brief An Array wrapper type that is responsible for holding an (ideally valid) pointer to an array on the GC heap.
/// Allows for C++ array semantics. Ex, [], begin(), end(), etc...
struct ArrayW {
    static_assert(sizeof(Ptr) == sizeof(void*), "Size of Ptr type must be the same as a void*!");

    /// @brief Create an ArrayW from a pointer
    constexpr ArrayW(Ptr initVal) noexcept : val(initVal) {}
    /// @brief Create an ArrayW from an arbitrary pointer
    constexpr ArrayW(void* alterInit) noexcept : val(reinterpret_cast<Ptr>(alterInit)) {}
    /// @brief Constructs an ArrayW that wraps a null value
    constexpr ArrayW(std::nullptr_t nptr) noexcept : val(nptr) {}
    /// @brief Default constructor wraps a nullptr array
    ArrayW() noexcept : val(nullptr) {}
    template<class U>
    requires (!std::is_same_v<std::nullptr_t, U> && std::is_convertible_v<U, T>)
    ArrayW(std::initializer_list<U> vals) : val(Array<T>::New(vals)) {}
    ArrayW(il2cpp_array_size_t size) : val(Array<T>::NewLength(size)) {}

    inline il2cpp_array_size_t Length() const noexcept {
        return val->Length();
    }
    inline il2cpp_array_size_t size() const noexcept {
        return val->Length();
    }
    T& operator[](size_t i) noexcept {
        return val->values[i];
    }
    const T& operator[](size_t i) const noexcept {
        return val->values[i];
    }

    inline void assertBounds(size_t i) {
        if (i < 0 || i >= Length()) {
            throw ArrayException(this, string_format("%zu is out of bounds for array of length: %zu", i, Length()));
        }
    }

    /// @brief Get a given index, performs bound checking and throws std::runtime_error on failure.
    /// @param i The index to get.
    /// @return The reference to the item.
    T& get(size_t i) {
        assertBounds(i);
        return val->values[i];
    }
    /// @brief Get a given index, performs bound checking and throws std::runtime_error on failure.
    /// @param i The index to get.
    /// @return The const reference to the item.
    const T& get(size_t i) const {
        assertBounds(i);
        return val->values[i];
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
    /// @brief Copies the array to the provided vector reference of same type.
    /// @param vec The vector to copy to.
    void copy_to(std::vector<T>& vec) const {
        vec.assign(val->values, val->values + Length());
    }
    /// @brief Provides a reference span of the held data within this array. The span should NOT outlive this instance.
    /// @return The created span.
    std::span<T> ref_to() {
        return std::span(val->values, Length());
    }
    /// @brief Provides a reference span of the held data within this array. The span should NOT outlive this instance.
    /// @return The created span.
    const std::span<T> ref_to() const {
        return std::span(const_cast<T*>(val->values), Length());
    }

    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = T const&;
    using iterator = pointer;
    using const_iterator = const_pointer;

    iterator begin() {
        return val->values;
    }
    iterator end() {
        return val->values + Length();
    }
    auto rbegin() {
        return std::reverse_iterator(val->values + Length());
    }
    auto rend() {
        return std::reverse_iterator(val->values);
    }
    const_iterator begin() const {
        return val->values;
    }
    const_iterator end() const {
        return val->values + Length();
    }
    auto rbegin() const {
        return std::reverse_iterator(val->values + Length());
    }
    auto rend() const {
        return std::reverse_iterator(val->values);
    }
    explicit operator const Ptr() const {
        return val;
    }
    explicit operator Ptr() {
        return val;
    }
    explicit operator Il2CppArray*() {
        return reinterpret_cast<Il2CppArray*>(val);
    }
    explicit operator Il2CppArray* const() const {
        return reinterpret_cast<Il2CppArray* const>(val);
    }
    constexpr const Ptr operator -> () const noexcept {
        return val;
    }
    constexpr Ptr operator -> () noexcept {
        return val;
    }
    template<class U>
    requires (std::is_convertible_v<U, T> || std::is_convertible_v<T, U>)
    operator ArrayW<U>() noexcept {
        return ArrayW<U>(reinterpret_cast<Array<U>*>(val));
    }
    template<class U>
    requires (std::is_convertible_v<U, T> || std::is_convertible_v<T, U>)
    operator ArrayW<U> const() const noexcept {
        return ArrayW<U>(reinterpret_cast<Array<U>* const>(val));
    }
    operator bool() const noexcept {
        return val != nullptr;
    }

    static ArrayW<T, Ptr> Empty() noexcept {
        return ArrayW<T, Ptr>(il2cpp_array_size_t(0));
    }

    T First() {
        if (Length() > 0)
            return val->values[0];
        else 
            throw ArrayException(this, "First called on empty array!");
    }

    template<typename D = T>
    requires(std::is_default_constructible_v<T>)
    T FirstOrDefault() {
        if (Length() > 0)
            return val->values[0];
        else return {};
    }

    T Last() {
        if (Length() > 0)
            return val->values[Length() - 1];
        else 
            throw ArrayException(this, "Last called on empty array!");
    }

    template<typename D = T>
    requires(std::is_default_constructible_v<T>)
    T LastOrDefault() {
        if (Length() > 0)
            return val->values[Length() - 1];
        else return {};
    }

    template<class Predicate>
    T First(Predicate pred) {
        for (iterator it = begin(); it != end(); it++) {
            if (pred(*it)) return *it;
        }
        throw ArrayException(this, "First on array found no value that matched predicate");
    }

    template<class Predicate>
    requires(std::is_default_constructible_v<T>)
    T FirstOrDefault(Predicate pred) {
        for (iterator it = begin(); it != end(); it++) {
            if (pred(*it)) return *it;
        }
        return {};
    }

    template<class Predicate>
    T Last(Predicate pred) {
        for (auto it = rbegin(); it != rend(); it++) {
            if (pred(*it)) return *it;
        }
        throw ArrayException(this, "Last on array found no value that matched predicate");
    }

    template<class Predicate>
    requires(std::is_default_constructible_v<T>)
    T LastOrDefault(Predicate pred) {
        for (auto it = rbegin(); it != rend(); it++) {
            if (pred(*it)) return *it;
        }
        return {};
    }

    constexpr void* convert() const noexcept {
        return val;
    }

    private:
    Ptr val;
};
static_assert(il2cpp_utils::has_il2cpp_conversion<ArrayW<int, Array<int>*>>);
template<class T, class Ptr>
struct ::il2cpp_utils::il2cpp_type_check::need_box<ArrayW<T, Ptr>> {
    constexpr static bool value = false;
};

template<class T, class Ptr>
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<ArrayW<T, Ptr>> {
    static inline Il2CppClass* get() {
        static auto klass = ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<Array<T>*>::get();
        return klass;
    }
};

#pragma pack(pop)
