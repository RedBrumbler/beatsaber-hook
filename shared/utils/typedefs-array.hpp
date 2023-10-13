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
    const void* arrayInstance;
    ArrayException(const void* instance, std::string_view msg) : il2cpp_utils::exceptions::StackTraceException(msg.data()), arrayInstance(instance) {}
};

/// @brief An Array wrapper type that is responsible for holding an (ideally valid) pointer to an array on the GC heap.
/// Allows for C++ array semantics. Ex, [], begin(), end(), etc...
template<typename T>
struct ArrayW;

template<typename T>
struct Array : Il2CppArray {
    // all value types and builtins are just T
    ALIGN_TYPE(8) T values[IL2CPP_ZERO_LEN_ARRAY];
    static_assert(::il2cpp_utils::is_il2cpp_size_safe_v<T>, "Il2Cpp size was not safe for type used in array!");

    /// @brief gets length of array, if multidimensional gets the array dimensionality
    constexpr il2cpp_array_size_t Length() const noexcept {
        if (bounds)
            return bounds->length;
        return max_length;
    }

    /// @throws ArrayException on out of bounds access
    inline void assertBounds(il2cpp_array_size_t i) const {
        if (i >= Length() || i < 0)
            throw ArrayException(this, "Out of bounds access on array instance!");
    }

    using value = T&;
    using const_value = const T&;

    using iterator = T*;
    using const_iterator = const T*;

    // unsafe indexing

    /// @brief index into array
    constexpr value operator[](std::size_t i) noexcept { return *internal_get(i); }
    /// @brief const index into array
    constexpr const_value operator[](std::size_t i) const noexcept { return *internal_get(i); }

    /// @brief index into array
    /// @throws ArrayException on out of bounds access
    // safe indexing
    inline value get(std::size_t i) {
        assertBounds(i);
        return *internal_get(i);
    }

    /// @brief index into array
    /// @throws ArrayException on out of bounds access
    inline const_value get(std::size_t i) const {
        assertBounds(i);
        return *internal_get(i);
    }

    // TODO: replace these optional with the wrapper ref stuff from bs hook
    /// @brief index into array
    /// @return nullopt for out of bounds access, IndexRef<T> otherwise
    inline std::optional<WrapperRef<T>> try_get(std::size_t i) noexcept {
        if (i >= Length() || i < 0) return std::nullopt;
        return WrapperRef<T>(*internal_get(i));
    }

    /// @brief index into array
    /// @return nullopt for out of bounds access, IndexRef<const T> otherwise
    inline std::optional<WrapperRef<const T>> try_get(std::size_t i) const noexcept {
        if (i >= Length() || i < 0) return std::nullopt;
        return WrapperRef<T>(*internal_get(i));
    }

    /// @brief creates array of length size and elements T
    /// @throws ArrayException if failed to make array
    static Array<T>* NewLength(il2cpp_array_size_t size) {
        il2cpp_functions::Init();
        auto arr = il2cpp_functions::array_new(
            il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<T>::get(),
            size
        );

        if (!arr) [[unlikely]] throw ::ArrayException(static_cast<Array<T>*>(nullptr), "Could not create Array!");
        return static_cast<Array<T>*>(arr);
    }

    /// @brief creates array with the length of the values provided, and fills it with those values
    /// @throws ArrayException if failed to make array
    static Array<T>* New(std::initializer_list<T> values) {
        // get an array of the right size
        auto arr = NewLength(values.size());
        memcpy(arr->values, &values[0], sizeof(T) * values.size());

        return arr;
    }

    /// @brief creates array with the length of the values provided, and fills it with those values
    /// @throws ArrayException if failed to make array
    template<typename... TArgs>
    static Array<T>* New(TArgs&&... args) {
        return New({args...});
    }

    /// @brief C# GetEnumerator method
    template<class U = Il2CppObject*>
    U GetEnumerator() {
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(
            this, "System.Collections.Generic.IEnumerable`1.GetEnumerator", 0));
        return ::il2cpp_utils::RunMethodRethrow<U, false>(this, method);
    }

    /// @brief C# Contains method
    bool Contains(T&& item) {
        // TODO: find a better way around the existence of 2 methods with this name (the 2nd not being generic at all)
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(
            this, "System.Collections.Generic.ICollection`1.Contains", 1));
        return ::il2cpp_utils::RunMethodRethrow<bool, false>(this, method, item);
    }

    /// @brief C# IndexOf method
    int IndexOf(T&& item) {
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(this, "System.Collections.Generic.IList`1.IndexOf", 1));
        return ::il2cpp_utils::RunMethodRethrow<int, false>(this, method, item);
    }

    /// @brief copies the contents of this array into the given array, starting at arrayIndex of the target array
    void CopyTo(::Array<T>* array, int arrayIndex) {
        if (arrayIndex + Length() > array->Length()) {
            throw ArrayException(this, "Tried copying data into array that is too small!");
        }

        for (auto src = begin(), dst = array->begin() + arrayIndex, e = end(); src != e; src++, dst++) {
            *dst = *src;
        }
    }

    /// @brief copies the contents of this array into the given vector
    inline void copy_into(std::vector<T>& vec) const {
        auto e = end();
        for (T const* ptr = values; ptr < e; ptr++) {
            vec.emplace_back(*ptr);
        }
    }

    // iterators

    /// @brief iterator at array start
    constexpr iterator begin() noexcept { return values; }
    /// @brief iterator at one past array end
    constexpr iterator end() noexcept { return values + Length(); }
    /// @brief iterator at reverse start
    inline auto rbegin() noexcept { return std::reverse_iterator(values + Length()); }
    /// @brief iterator at one past reverse end
    inline auto rend() noexcept { return std::reverse_iterator(values); }

    /// @brief const iterator at array start
    constexpr const_iterator begin() const noexcept { return values; }
    /// @brief const iterator at one past array end
    constexpr const_iterator end() const noexcept { return values + Length(); }
    /// @brief const iterator at reverse start
    inline auto rbegin() const noexcept { return std::reverse_iterator(values + Length()); }
    /// @brief const iterator at one past reverse end
    inline auto rend() const noexcept { return std::reverse_iterator(values); }

    /// @brief attempts to get the first element of the array
    /// @throws ArrayException if array is length 0
    T First() const {
        if (Length() > 0)
            return *internal_get(0);

        throw ArrayException(this, "Tried getting First from array with Length 0");
    }

    /// @brief attempts to get the first element of the array
    /// @return first element, or nullptr constructed if length is 0
    constexpr T FirstOrDefault() const noexcept {
        if (Length() > 0)
            return *internal_get(0);
        return {};
    }

    /// @brief attempts to get the last element of the array
    /// @throws ArrayException if array is length 0
    T Last() const {
        if (Length() > 0)
            return *internal_get(Length() - 1);

        throw ArrayException(this, "Tried getting First from array with Length 0");
    }

    /// @brief attempts to get the last element of the array
    /// @return last element, or nullptr constructed if length is 0
    constexpr T LastOrDefault() const noexcept {
        if (Length() > 0)
            return *internal_get(Length() - 1);
        return {};
    }

    /// @brief attempts to get the first element of the array that matches the given predicate
    /// @throws ArrayException if not found
    template<typename Pred>
    T First(Pred predicate) const {
        auto e = end();
        for (auto itr = begin(); itr != e; itr++) {
            if (predicate(*itr)) return *itr;
        }

        throw ArrayException(this, "Tried getting First from array with Length 0");
    }

    /// @brief attempts to get the first element of the array that matches the given predicate
    /// @return first element that matches, or nullptr constructed if not found
    template<typename Pred>
    T FirstOrDefault(Pred predicate) const {
        auto e = end();
        for (auto itr = begin(); itr != e; itr++) {
            if (predicate(*itr)) return *itr;
        }
        return {};
    }

    /// @brief attempts to get the last element of the array that matches the given predicate
    /// @throws ArrayException if not found
    template<typename Pred>
    T Last(Pred predicate) const {
        auto e = rend();
        for (auto itr = rbegin(); itr != e; itr++) {
            if (predicate(*itr)) return *itr;
        }

        throw ArrayException(this, "Tried getting First from array with Length 0");
    }

    /// @brief attempts to get the last element of the array that matches the given predicate
    /// @return last element that matches, or nullptr constructed if not found
    template<typename Pred>
    T LastOrDefault(Pred predicate) const {
        auto e = rend();
        for (auto itr = rbegin(); itr != e; itr++) {
            if (predicate(*itr)) return *itr;
        }
        return {};
    }
    /// @brief span of the backing T array
    std::span<T> ref_to() { return std::span(values, Length()); }
    /// @brief span of the backing T array
    std::span<const T> ref_to() const { return std::span(const_cast<T*>(values), Length()); }

    operator Il2CppArray* () noexcept { return this; }
    operator const Il2CppArray* () const noexcept { return this; }

    protected:
        friend ArrayW<T>;
        /// @brief gets the address of the value @ index i
        T* internal_get(std::size_t i) { return (values + i); }
        /// @brief gets the address of the value @ index i
        T const* internal_get(std::size_t i) const { return (values + i); }
};

/// @brief type for ref type arrays to keep a reference to an index in the array
template<typename T>
requires(::il2cpp_utils::il2cpp_reference_type<std::remove_const_t<T>>)
struct IndexRef {
    // ref to the array this came from, only really used for set 'field', verify if needed and if not, remove
    Il2CppObject* const obj;
    // pointer to within the Array::values array, or at least it's supposed to be
    void** _value;

    /// @brief gets a wrapped void* for this value as T
    constexpr T get() const noexcept { return T(convert()); }

    /// @brief set the backing void*
    void set(T&& v) const {
        static_assert(!std::is_const_v<T>);
        ::il2cpp_functions::Init();
        *_value = v.convert();
        // whenever assigning something that contains refs, you need to set the write barrier!
        ::il2cpp_functions::GarbageCollector_SetWriteBarrier(_value);
    }

    /// @brief operator eq
    template<typename U = T>
    requires(std::is_convertible_v<T, U> || std::is_convertible_v<U, T>)
    constexpr bool operator==(const IndexRef<U>& o) const noexcept { return _value == o._value; }
    /// @brief operator lt
    template<typename U = T>
    requires(std::is_convertible_v<T, U> || std::is_convertible_v<U, T>)
    constexpr bool operator<(const IndexRef<U>& o) const noexcept { return _value < o._value; }
    /// @brief operator gt
    template<typename U = T>
    requires(std::is_convertible_v<T, U> || std::is_convertible_v<U, T>)
    constexpr bool operator>(const IndexRef<U>& o) const noexcept { return _value > o._value; }
    /// @brief checks the backing void* for null TODO: should it check unity objects for cached ptr?
    constexpr operator bool() const { return *_value; }

    /// @brief struct to help with the operator ->
    struct ArrowHelper {
        T v;
        constexpr T* operator ->() { return &v; }
        constexpr const T* operator ->() const { return &v; }
    };

    /// @brief operator-> to directly access fields and values on the backed void*
    ArrowHelper operator ->() const noexcept { return ArrowHelper { get() }; }

    /// @brief assign operator to assign the backing void*
    template<typename U = T>
    requires(std::is_convertible_v<U, T>)
    constexpr const IndexRef& operator=(U&& v) const { set(T(v)); return *this; }

    /// @brief convert operator to anything T can just turn into
    template<typename U = T>
    requires(std::is_convertible_v<T, U>)
    constexpr operator U() const noexcept { return get(); }

    /// @brief deref operator
    template<typename U = T>
    requires(std::is_convertible_v<T, U>)
    constexpr U operator*() const noexcept { return get(); }

    /// @brief il2cpp convert method
    constexpr void* convert() const { return *_value; }
};

/// @brief wrapper for an iter over a ref type array, inherits indexref since a lot would remain the same
template<typename T>
requires(::il2cpp_utils::il2cpp_reference_type<std::remove_const_t<T>>)
struct Iter : public IndexRef<T> {
    // 90% of the code would be the same as the IndexRef<T>, so we just reuse that and also implement iterator methods on top of that
    Iter& operator ++() { IndexRef<T>::_value++; return *this; }
    Iter& operator --() { IndexRef<T>::_value--; return *this; }
    Iter& operator ++(int) { ++IndexRef<T>::_value; return *this; }
    Iter& operator --(int) { --IndexRef<T>::_value; return *this; }

    Iter& operator +=(std::size_t i) { IndexRef<T>::_value += i; return *this; }
    Iter& operator -=(std::size_t i) { IndexRef<T>::_value -= i; return *this; }

    Iter operator +(std::size_t i) const { return IndexRef<T>(IndexRef<T>::obj, IndexRef<T>::_value + i); }
    Iter operator -(std::size_t i) const { return IndexRef<T>(IndexRef<T>::obj, IndexRef<T>::_value + i); }
};

template<::il2cpp_utils::il2cpp_reference_type T>
// if il2cpp size happens to be right for the wrapper type, fall back to the general T implementation
requires(!::il2cpp_utils::is_il2cpp_size_safe_v<T>)
struct Array<T> : ::Il2CppArray {
    // ref types are void*
    ALIGN_TYPE(8) void* values[IL2CPP_ZERO_LEN_ARRAY];

    /// @brief gets length of array, if multidimensional gets the array dimensionality
    constexpr il2cpp_array_size_t Length() const noexcept {
        if (bounds)
            return bounds->length;
        return max_length;
    }

    /// @throws ArrayException on out of bounds access
    inline void assertBounds(il2cpp_array_size_t i) const {
        if (i >= Length() || i < 0)
            throw ArrayException(this, "Out of bounds access on array instance!");
    }

    using value = IndexRef<T>;
    using const_value = IndexRef<const T>;

    using iterator = Iter<T>;
    using const_iterator = Iter<const T>;

    // unsafe indexing

    /// @brief index into array
    constexpr value operator[](std::size_t i) noexcept { return value{this, values + i}; }
    /// @brief const index into array
    constexpr const_value operator[](std::size_t i) const noexcept { return const_value{this, values + i}; }

    // safe indexing

    /// @brief index into array
    /// @throws ArrayException on out of bounds access
    inline value get(std::size_t i) {
        assertBounds(i);
        return value{this, values + i};
    }

    /// @brief index into array
    /// @throws ArrayException on out of bounds access
    inline const_value get(std::size_t i) const {
        assertBounds(i);
        return const_value{this, values + i};
    }

    /// @brief index into array
    /// @return nullopt for out of bounds access, IndexRef<T> otherwise
    inline std::optional<value> try_get(std::size_t i) noexcept {
        if (i >= Length() || i < 0) return std::nullopt;
        return value{this, values + i};
    }

    /// @brief index into array
    /// @return nullopt for out of bounds access, IndexRef<const T> otherwise
    inline std::optional<const_value> try_get(std::size_t i) const noexcept {
        if (i >= Length() || i < 0) return std::nullopt;
        return const_value{this, values + i};
    }

    /// @brief creates array of length size and elements T
    /// @throws ArrayException if failed to make array
    static Array<T>* NewLength(il2cpp_array_size_t size) {
        il2cpp_functions::Init();
        auto arr = il2cpp_functions::array_new(
            il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<T>::get(),
            size
        );

        if (!arr) [[unlikely]] throw ::ArrayException(static_cast<Array<T>*>(nullptr), "Could not create Array!");
        return static_cast<Array<T>*>(arr);
    }

    /// @brief creates array with the length of the values provided, and fills it with those values
    /// @throws ArrayException if failed to make array
    static Array<T>* New(std::initializer_list<T> values) {
        // get an array of the right size
        auto arr = NewLength(values.size());

        void** cur = arr->values;
        // we can't do a memcpy
        for (const auto& v : values) {
            *cur = v.convert();
            cur++;
        }

        return arr;
    }

    /// @brief creates array with the length of the values provided, and fills it with those values
    /// @throws ArrayException if failed to make array
    template<typename... TArgs>
    static Array<T>* New(TArgs&&... args) {
        return New({args...});
    }

    /// @brief C# GetEnumerator method
    template<class U = Il2CppObject*>
    U GetEnumerator() {
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(
            this, "System.Collections.Generic.IEnumerable`1.GetEnumerator", 0));
        return ::il2cpp_utils::RunMethodRethrow<U, false>(this, method);
    }

    /// @brief C# Contains method
    bool Contains(T&& item) {
        // TODO: find a better way around the existence of 2 methods with this name (the 2nd not being generic at all)
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(
            this, "System.Collections.Generic.ICollection`1.Contains", 1));
        return ::il2cpp_utils::RunMethodRethrow<bool, false>(this, method, item);
    }

    /// @brief C# IndexOf method
    int IndexOf(T&& item) {
        static auto* method = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(this, "System.Collections.Generic.IList`1.IndexOf", 1));
        return ::il2cpp_utils::RunMethodRethrow<int, false>(this, method, item);
    }

    /// @brief copies the contents of this array into the given array, starting at arrayIndex of the target array
    void CopyTo(::Array<T>* array, int arrayIndex) {
        if (arrayIndex + Length() > array->Length()) {
            throw ArrayException(this, "Tried copying data into array that is too small!");
        }

        for (auto src = begin(), dst = array->begin() + arrayIndex, e = end(); src != e; src++, dst++) {
            dst = src;
        }
    }

    /// @brief copies the contents of this array into the given vector
    inline void copy_into(std::vector<T>& vec) const {
        auto e = values + Length();
        for (void* const* ptr = values; ptr != e; ptr++) {
            vec.emplace_back(*ptr);
        }
    }

    // iterators

    /// @brief iterator at array start
    constexpr iterator begin() noexcept { return iterator{this, values}; }
    /// @brief iterator at one past array end
    constexpr iterator end() noexcept { return iterator{this, values + Length()}; }
    /// @brief iterator at reverse start
    inline auto rbegin() noexcept { return std::reverse_iterator<iterator>({this, values + Length()}); }
    /// @brief iterator at one past reverse end
    inline auto rend() noexcept { return std::reverse_iterator<iterator>({this, values}); }

    /// @brief const iterator at array start
    constexpr const_iterator begin() const noexcept { return const_iterator({this, values}); }
    /// @brief const iterator at one past array end
    constexpr const_iterator end() const noexcept { return const_iterator({this, values + Length()}); }
    /// @brief const iterator at reverse start
    inline auto rbegin() const noexcept { return std::reverse_iterator<const_iterator>({this, values + Length()}); }
    /// @brief const iterator at one past reverse end
    inline auto rend() const noexcept { return std::reverse_iterator<const_iterator>({this, values}); }

    // search methods

    /// @brief attempts to get the first element of the array
    /// @throws ArrayException if array is length 0
    T First() const {
        if (Length() > 0)
            return T(*values);

        throw ArrayException(this, "Tried getting First from array with Length 0");
    }

    /// @brief attempts to get the first element of the array
    /// @return first element, or nullptr constructed if length is 0
    constexpr T FirstOrDefault() const noexcept {
        if (Length() > 0)
            return T(*values);
        return T(static_cast<void*>(nullptr));
    }

    /// @brief attempts to get the last element of the array
    /// @throws ArrayException if array is length 0
    T Last() const {
        if (Length() > 0)
            return T(*(values + Length() - 1));

        throw ArrayException(this, "Tried getting First from array with Length 0");
    }

    /// @brief attempts to get the last element of the array
    /// @return last element, or nullptr constructed if length is 0
    constexpr T LastOrDefault() const noexcept {
        if (Length() > 0)
            return T(*(values + Length() - 1));
        return T(static_cast<void*>(nullptr));
    }

    /// @brief attempts to get the first element of the array that matches the given predicate
    /// @throws ArrayException if not found
    template<typename Pred>
    T First(Pred predicate) const {
        auto e = end();
        for (auto itr = begin(); itr != e; itr++) {
            if (predicate(*itr)) return *itr;
        }

        throw ArrayException(this, "Tried getting First from array with Length 0");
    }

    /// @brief attempts to get the first element of the array that matches the given predicate
    /// @return first element that matches, or nullptr constructed if not found
    template<typename Pred>
    T FirstOrDefault(Pred predicate) const {
        auto e = end();
        for (auto itr = begin(); itr != e; itr++) {
            if (predicate(*itr)) return *itr;
        }
        return T(static_cast<void*>(nullptr));
    }

    /// @brief attempts to get the last element of the array that matches the given predicate
    /// @throws ArrayException if not found
    template<typename Pred>
    T Last(Pred predicate) const {
        auto e = rend();
        for (auto itr = rbegin(); itr != e; itr++) {
            if (predicate(*itr)) return *itr;
        }

        throw ArrayException(this, "Tried getting First from array with Length 0");
    }

    /// @brief attempts to get the last element of the array that matches the given predicate
    /// @return last element that matches, or nullptr constructed if not found
    template<typename Pred>
    T LastOrDefault(Pred predicate) const {
        auto e = rend();
        for (auto itr = rbegin(); itr != e; itr++) {
            if (predicate(*itr)) return *itr;
        }
        return T(static_cast<void*>(nullptr));
    }

    /// @brief span of the backing void* array
    std::span<void*> ref_to() { return std::span(values, Length()); }
    /// @brief span of the backing void* array
    std::span<const void*> ref_to() const { return std::span(const_cast<void**>(values), Length()); }

    operator Il2CppArray* () noexcept { return this; }
    operator const Il2CppArray* () const noexcept { return this; }
    protected:
        friend ArrayW<T>;
        /// @brief gets the address of the value @ index i
        void** internal_get(std::size_t i) { return (values + i); }
        /// @brief gets the address of the value @ index i
        void* const* internal_get(std::size_t i) const { return (values + i); }
};

template<typename T>
struct ArrayW {
    using Ptr = Array<T>*;
    /// @brief default constructor
    constexpr ArrayW() noexcept : val(nullptr) {}
    /// @brief explicit construction from a void*
    constexpr explicit ArrayW(void* i) noexcept : val(static_cast<Ptr>(i)) {}
    /// @brief construct an array from Array<T> pointer
    constexpr ArrayW(Ptr i) noexcept : val(i) {}
    /// @brief construct an array from nullptr
    constexpr ArrayW(std::nullptr_t nptr) noexcept : val(nptr) {}

    template<typename U = T>
    requires(std::is_convertible_v<U, T> || std::is_same_v<U, T>)
    explicit constexpr ArrayW(ArrayW<U> i) noexcept : val(static_cast<Ptr>(i.convert())) {}

    /// @brief constructor that creates an array of size sz
    inline ArrayW(il2cpp_array_size_t sz) : val(Array<T>::NewLength(sz)) {}

    /// @brief constructor that takes a set of U values that can convert into T (or are T), and builds an array from it
    template<typename U = T>
    requires(std::is_convertible_v<U, T> || std::is_same_v<U, T>)
    inline ArrayW(std::initializer_list<U> vals) : val(Ptr::New(vals)) {}

    /// @brief gets length of array, if multidimensional gets the array dimensionality
    constexpr il2cpp_array_size_t Length() const noexcept { return val->Length(); }
    /// @brief gets length of array, if multidimensional gets the array dimensionality instead
    constexpr il2cpp_array_size_t size() const noexcept { return val->Length(); }

    /// @throws ArrayException on out of bounds access
    inline void assertBounds() const { return val->assertBounds(); }

    /// @brief il2cpp conversion method
    constexpr void* convert() const noexcept { return const_cast<void*>(static_cast<const void*>(val)); }

    /// @brief index into array
    constexpr decltype(auto) operator[](std::size_t i) noexcept { return val->operator[](i); }
    /// @brief const index into array
    constexpr decltype(auto) operator[](std::size_t i) const noexcept { return val->operator[](i); }

    /// @brief index into array
    /// @throws ArrayException on out of bounds access
    inline decltype(auto) get(std::size_t i) { return val->get(i); }
    /// @brief index into array
    /// @throws ArrayException on out of bounds access
    inline decltype(auto) get(std::size_t i) const { return val->get(i); }

    /// @brief index into array
    /// @return nullopt for out of bounds access, value otherwise
    inline decltype(auto) try_get(std::size_t i) noexcept { return val->try_get(i); }
    /// @brief index into array
    /// @return nullopt for out of bounds access, const value otherwise
    inline decltype(auto) try_get(std::size_t i) const noexcept { return val->try_get(i); }

    /// @brief C# GetEnumerator method
    template<class U = Il2CppObject*>
    inline U GetEnumerator() { return val->template GetEnumerator<U>(); }
    /// @brief C# Contains method
    inline bool Contains(T&& item) { return val->Contains(item); }
    /// @brief C# IndexOf method
    inline int IndexOf(T&& item) { return val->IndexOf(item); }
    /// @brief copies the contents of this array into the given array, starting at arrayIndex of the target array
    inline void CopyTo(::Array<T>* array, int arrayIndex) { return val->CopyTo(array, arrayIndex); }
    /// @brief copies the contents of this array into the given vector
    inline void copy_into(std::vector<T>& vec) const { return val->copy_into(vec); }

    // iterators

    /// @brief iterator at array start
    constexpr decltype(auto) begin() noexcept { return val->begin(); }
    /// @brief iterator at one past array end
    constexpr decltype(auto) end() noexcept { return val->begin(); }
    /// @brief iterator at reverse start
    inline decltype(auto) rbegin() noexcept { return val->rbegin(); }
    /// @brief iterator at one past reverse end
    inline decltype(auto) rend() noexcept { return val->rend(); }

    /// @brief const iterator at array start
    constexpr decltype(auto) begin() const noexcept { return val->begin(); }
    /// @brief const iterator at one past array end
    constexpr decltype(auto) end() const noexcept { return val->begin(); }
    /// @brief const iterator at reverse start
    inline decltype(auto) rbegin() const noexcept { return val->rbegin(); }
    /// @brief const iterator at one past reverse end
    inline decltype(auto) rend() const noexcept { return val->rend(); }

    // search methods

    /// @brief attempts to get the first element of the array
    /// @throws ArrayException if array is length 0
    inline decltype(auto) First() const { return val->First(); }
    /// @brief attempts to get the first element of the array
    /// @return first element, or nullptr constructed if length is 0
    constexpr decltype(auto) FirstOrDefault() const noexcept { return val->FirstOrDefault(); }
    /// @brief attempts to get the last element of the array
    /// @throws ArrayException if array is length 0
    inline decltype(auto) Last() const { return val->Last(); }
    /// @brief attempts to get the last element of the array
    /// @return last element, or nullptr constructed if length is 0
    constexpr decltype(auto) LastOrDefault() const noexcept { return val->LastOrDefault(); }

    /// @brief attempts to get the first element of the array that matches the given predicate
    /// @throws ArrayException if not found
    template<typename Pred>
    inline decltype(auto) First(Pred predicate) const { return val->First(predicate); }

    /// @brief attempts to get the first element of the array that matches the given predicate
    /// @return first element that matches, or nullptr constructed if not found
    template<typename Pred>
    inline decltype(auto) FirstOrDefault(Pred predicate) const { return val->FirstOrDefault(predicate); }

    /// @brief attempts to get the last element of the array that matches the given predicate
    /// @throws ArrayException if not found
    template<typename Pred>
    inline decltype(auto) Last(Pred predicate) const { return val->Last(predicate); }

    /// @brief attempts to get the last element of the array that matches the given predicate
    /// @return last element that matches, or nullptr constructed if not found
    template<typename Pred>
    inline decltype(auto) LastOrDefault(Pred predicate) const { return val->LastOrDefault(predicate); }

    /// @brief span of the backing array values
    inline decltype(auto) ref_to() { return val->ref_to(); }
    /// @brief span of the backing array values
    inline decltype(auto) ref_to() const { return val->ref_to(); }

    /// @brief shorthand for creating an array T with size 0
    static inline ArrayW<T> Empty() { return ArrayW<T>(il2cpp_array_size_t(0)); }

    // FIXME: this is actually not valid C# conversion, should we construct a new array instead?
    template<typename U>
    requires(std::is_convertible_v<T, U>)
    constexpr explicit operator ArrayW<U>() { return ArrayW<T>(convert()); }

    /// @brief conversion into backing pointer
    constexpr explicit operator Il2CppArray*() { return val; }
    /// @brief conversion into backing pointer
    constexpr explicit operator const Il2CppArray*() const { return val; }

    /// @brief conversion into backing pointer
    constexpr explicit operator Ptr() { return val; }
    /// @brief conversion into backing pointer
    constexpr explicit operator const Ptr() const { return val; }

    /// @brief member access into pointer through operator->
    constexpr Ptr operator ->() { return val; }
    /// @brief member access into pointer through operator->
    constexpr const Ptr operator ->() const { return val; }

    explicit operator void*() const noexcept { return convert(); }

    operator bool() const { return val; }
    protected:
        Ptr val;
};

MARK_GEN_REF_T(ArrayW);

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
