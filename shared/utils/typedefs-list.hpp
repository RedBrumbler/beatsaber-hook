#pragma once
// This file should only ever be included in typedefs.h

#include <sys/types.h>
#include <optional>
#include <span>
#include <stdexcept>
#include <utility>
#include "il2cpp-utils-methods.hpp"
#include "type-concepts.hpp"


#include "typedefs-array.hpp"
#include "typedefs-object.hpp"

/// @brief Represents an exception thrown from usage of a List
struct ListException : il2cpp_utils::exceptions::StackTraceException {
    void* arrayInstance;
    ListException(void* instance, std::string_view msg) : il2cpp_utils::exceptions::StackTraceException(msg.data()), arrayInstance(instance) {}
};

// System.Collections.Generic.List
template <class T>
struct List : Il2CppObject {
    Array<T>* _items;
    int _size;
    int _version;
    Il2CppObject* _syncRoot;

#ifdef BS_HOOK_LIST_USE_IL2CPP
    // Add an item to this list
    void Add(T item) {
        if (!static_cast<void*>(this)) throw ListException(nullptr, "Running instance method on nullptr instance!");
        il2cpp_functions::Init();

        auto klass = il2cpp_functions::object_get_class(this);
        auto* ___internal_method =
            THROW_UNLESS(il2cpp_utils::FindMethod(klass, "Add", std::vector<Il2CppClass*>{}, ::std::vector<const Il2CppType*>{ ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_type<T>::get() }));
        il2cpp_utils::RunMethodRethrow<void, false>(this, ___internal_method, item);
    }

    // Ensure the capacity of this list
    void EnsureCapacity(int min) {
        if (!static_cast<void*>(this)) throw ListException(nullptr, "Running instance method on nullptr instance!");
        il2cpp_functions::Init();

        auto klass = il2cpp_functions::object_get_class(this);
        auto* ___internal_method = THROW_UNLESS(
            il2cpp_utils::FindMethod(klass, "EnsureCapacity", std::vector<Il2CppClass*>{}, ::std::vector<const Il2CppType*>{ ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_type<int32_t>::get() }));

        il2cpp_utils::RunMethodRethrow<void, false>(this, ___internal_method, min);
    }
#else   
    void Insert(int index, T item) {
        if (index > this->size()) {
            throw std::runtime_error("Capacity size too small");
        }
        if (_size == this->get_items().size()) {
            this->EnsureCapacity(this->size() + 1);
        }
        if (index < this->size()) {
            std::copy(this->begin() + index, this->begin() + index + 1, this->begin() + (this->size() - index));
            // Array.Copy(this._items, index, this._items, index + 1,
            //            this._size - index);
        }
        this->_items[index] = item;
        this->_size++;
        this->_version++;
    }
    void Add(T item) {
        this->_version++;
        auto items = this->get_items();
        auto size = this->size();
        if (size < items.Length) {
            this->_size = size + 1;
            items[size] = item;
            return;
        }
        this->AddWithResize(item);
    }

    void Clear() {
        this->_version++;
        if constexpr (il2cpp_utils::il2cpp_reference_type<T>) {
            int size = this->_size;
            this->_size = 0;
            if (size > 0) {
                std::fill(this->_items.begin(), this->items.begin() + this->_size, {});
                return;
            }
        } else {
            this->_size = 0;
        }
    }

    int IndexOf(T item) {
        auto start = this->items.begin();
        auto end = this->items.begin() + this->_size;
        auto it = std::find(start, end, item);

        if (it == end) return -1;

        return std::distance(start, it);
    }

    ArrayW<T> ToArray() {
        ArrayW<T> newArr = ArrayW<T>(this->_size);
        std::copy(this->_items.begin(), this->_items.begin() + _size, newArr.begin());

        return newArr;
    }

   protected:
    void SetCapacity(int value) {
        if (value < this->_size) {
            throw std::runtime_error("Capacity size too small");
        }
        if (value != this->_items.size()) {
            if (value > 0) {
                auto array = ArrayW<T>(value);
                if (this->size() > 0) {
                    std::copy(this->begin(), this->end(), array.begin());
                }
                this->_items = array;
                return;
            }
            this->_items = ArrayW<T>(0);
        }
    }

    void AddWithResize(T item) {
        auto size = this->size();
        this->EnsureCapacity(size + 1);
        this->_size = size + 1;
        this->_items[size] = item;
    }

    void EnsureCapacity(int min) {
        if (this->_items->size() < min) {
            int num = (this->_items.size() == 0) ? 4 : (this->_items.size() * 2);
            if (num > 2146435071) {
                num = 2146435071;
            }
            if (num < min) {
                num = min;
            }
            SetCapacity(num);
        }
    }
#endif
};
DEFINE_IL2CPP_ARG_TYPE_GENERIC_CLASS(List, "System.Collections.Generic", "List`1");

template <class T, class Ptr = List<T>*>
struct ListWrapper {
    static_assert(sizeof(Ptr) == sizeof(void*), "Size of Ptr type must be the same as a void*!");
    using WrappedType = Ptr;

    constexpr ListWrapper() noexcept : ptr(nullptr) {}

    // TODO: Consider requirementally constexpr-ifying this call
    // TODO: Apply these il2cpp conversion changes to ArrayW as well, to permit
    // ArrayW to hold wrapper types and not pure pointers
    constexpr ListWrapper(Ptr const& p) noexcept : ptr(p) {}
    constexpr ListWrapper(Ptr&& p) noexcept : ptr(p) {}

    template <class V = void>
        requires(std::is_pointer_v<Ptr> && !il2cpp_utils::has_il2cpp_conversion<Ptr>)
    constexpr ListWrapper(void* alterInit) noexcept : ptr(reinterpret_cast<Ptr>(alterInit)) {}

    constexpr ListWrapper(std::span<T> const p) : ptr(il2cpp_utils::NewSpecific<Ptr>(p.size())) {
        std::copy(p.begin(), p.end(), begin());
    }

    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;

    using iterator = pointer;
    using const_iterator = const_pointer;

    [[nodiscard]] constexpr int size() const {
        return ptr->_size;
    }
    T& operator[](size_t i) {
        return get_items()->values[i];
    }
    const T& operator[](size_t i) const {
        return get_items()->values[i];
    }

    /// @brief Get a given index, performs bound checking and throws
    /// std::runtime_error on failure.
    /// @param i The index to get.
    /// @return The reference to the item.
    T& get(size_t i) {
        THROW_UNLESS(i < size() && i >= 0);
        return get_items()->values[i];
    }
    /// @brief Get a given index, performs bound checking and throws
    /// std::runtime_error on failure.
    /// @param i The index to get.
    /// @return The const reference to the item.
    const T& get(size_t i) const {
        THROW_UNLESS(i < size() && i >= 0);
        return get_items()->values[i];
    }
    /// @brief Tries to get a given index, performs bound checking and returns a
    /// std::nullopt on failure.
    /// @param i The index to get.
    /// @return The WrapperRef<T> to the item, mostly considered to be a T&.
    std::optional<WrapperRef<T>> try_get(size_t i) {
        if (i >= size() || i < 0) {
            return std::nullopt;
        }
        return WrapperRef(get_items()->values[i]);
    }
    /// @brief Tries to get a given index, performs bound checking and returns a
    /// std::nullopt on failure.
    /// @param i The index to get.
    /// @return The WrapperRef<const T> to the item, mostly considered to be a
    /// const T&.
    std::optional<WrapperRef<const T>> try_get(size_t i) const {
        if (i >= size() || i < 0) {
            return std::nullopt;
        }
        return WrapperRef(get_items()->values[i]);
    }

    iterator begin() {
        return get_items()->values;
    }
    iterator end() {
        return &get_items()->values[size()];
    }
    const_iterator begin() const {
        return get_items()->values;
    }
    const_iterator end() const {
        return &get_items()->values[size()];
    }

    operator std::span<T const> const() const {
        return std::span<T const>(this->begin(), this->end());
    }

    operator std::span<T>() {
        return std::span<T>(this->begin(), this->end());
    }

    operator Ptr() noexcept {
        return ptr;
    };

    constexpr void* convert() const noexcept {
        if constexpr (std::is_pointer_v<Ptr>) {
            return ptr;
        } else if constexpr (il2cpp_utils::has_il2cpp_conversion<Ptr>) {
            return ptr.convert();
        }
    }

    Ptr operator->() noexcept {
        return ptr;
    }

    Ptr const operator->() const noexcept {
        return ptr;
    }

    constexpr ListWrapper& operator=(Ptr const& ptr) {
        this->ptr = ptr;
    }
    constexpr ListWrapper& operator=(Ptr&& ptr) {
        this->ptr = ptr;
    }

    // method to create a new list easily
    template <il2cpp_utils::CreationType creationType = il2cpp_utils::CreationType::Temporary>
    static ListWrapper<T, Ptr> New() {
        il2cpp_functions::Init();
        auto ls = il2cpp_utils::New<Ptr, creationType>();
        if (!ls) throw ListException(nullptr, "Could not create list!");

        return { *ls };
    }

    // method to create a new list easily
    template <typename U, il2cpp_utils::CreationType creationType = il2cpp_utils::CreationType::Temporary>
        requires(std::is_convertible_v<U, T>)
    static ListWrapper<T, Ptr> New(std::initializer_list<U> values) {
        il2cpp_functions::Init();
        auto ls = il2cpp_utils::New<Ptr, creationType>();
        if (!ls) throw ListException(nullptr, "Could not create list!");

        ls->EnsureCapacity(values.size());
        for (auto& v : values) ls->Add(v);

        return { *ls };
    }

    std::optional<uint_t> index_of(T item) {
        auto index = this->as_il2cpp_list()->IndexOf(item);
        if (index < 0) return std::nullopt;

        return index;
    }

    void clear() {
        this->as_il2cpp_list()->Clear();
    }

    void insert_at(int index, T item) {
        this->as_il2cpp_list()->Insert(index, item);
    }
    void push_back(T item) {
        this->as_il2cpp_list()->Add(item);
    }

    template <typename... TArgs>
    requires(il2cpp_utils::il2cpp_reference_type<T>)
    void emplace_back(TArgs&&... args) {
        this->as_il2cpp_list()->Add(T::New_ctor(std::forward<TArgs>(args)...));
    }
    
    template <typename... TArgs>
    void emplace_back(TArgs&&... args) {
        this->as_il2cpp_list()->Add(T(std::forward<TArgs>(args)...));
    }

   private:
    auto get_items() const {
        return ptr->_items;
    }
    auto get_items() {
        return ptr->_items;
    }

    auto as_il2cpp_list() {
        return reinterpret_cast<List<T>*>(ptr);
    }

    Ptr ptr;
};
MARK_GEN_REF_T(ListWrapper);
MARK_GEN_REF_PTR_T(List);

static_assert(il2cpp_utils::has_il2cpp_conversion<ListWrapper<int, List<int>*>>);
template <class T, class Ptr>
struct BS_HOOKS_HIDDEN ::il2cpp_utils::il2cpp_type_check::need_box<ListWrapper<T, Ptr>> {
    constexpr static bool value = false;
};

template <typename T, template <typename U = T> typename Wrapper>
ListWrapper(Wrapper<T>) -> ListWrapper<T, Wrapper<T>>;

// if system list exists, we can use it in ListW, but with a compile definition
// it can be disabled
#if !defined(NO_CODEGEN_WRAPPERS) && __has_include("System/Collections/Generic/List_1.hpp")
namespace System::Collections::Generic {
template <typename T>
class List_1;
}
// forward declare usage
template <typename T, typename Ptr = System::Collections::Generic::List_1<T>*>
using ListW = ListWrapper<T, Ptr>;

// include header
#include "System/Collections/Generic/List_1.hpp"
#else
template <typename T, typename Ptr = List<T>*>
using ListW = ListWrapper<T, Ptr>;
#endif

static_assert(il2cpp_utils::has_il2cpp_conversion<ListW<int>>);
