#pragma once
// This file should only ever be included in typedefs.h

#include <sys/types.h>
#include <algorithm>
#include <iterator>
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
    ArrayW<T> _items;
    int _size;
    int _version;
    Il2CppObject* _syncRoot;

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
        return this->ptr->_size;
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

        ListWrapper<T, Ptr> lsWrap = { *ls };

        lsWrap.insert_range(values);

        return lsWrap;
    }

    /**
     * @brief System.Collections.Generic::List<T>.IndexOf(T item)
     *
     * https://learn.microsoft.com/en-us/dotnet/api/system.collections.generic.list-1.indexof?view=net-8.0
     * @param index
     * @param item
     */
    std::optional<uint_t> index_of(T item) {
        auto start = this->begin();
        auto end = this->begin() + this->size();
        auto it = std::find(start, end, item);

        if (it == end) return std::nullopt;

        return std::distance(start, it);
    }

    template <typename F>
    std::optional<T> First(F&& func) {
        auto start = this->begin();
        auto end = this->begin() + this->size();
        auto it = std::find_if(start, end, std::forward<F>(func));

        if (it == end) return std::nullopt;

        return *it;
    }

    template <typename F>
    T FirstOrDefault(F&& func) {
        return First(func).value_or(T());
    }

    template <typename F>
    std::optional<T> Last(F&& func) {
        auto start = this->begin();
        auto end = this->begin() + this->size();

        auto rev_start = std::make_reverse_iterator(start);
        auto rev_end = std::make_reverse_iterator(end);

        auto it = std::find_if(rev_start, rev_end, std::forward<F>(func));

        if (it == rev_end) return std::nullopt;

        return *it;
    }

    template <typename F>
    T LastOrDefault(F&& func) {
        return Last(func).value_or(T());
    }

    /**
     * @brief System.Collections.Generic::List<T>.Clear()
     *
     * https://learn.microsoft.com/en-us/dotnet/api/system.collections.generic.list-1.clear?view=net-8.0
     * @param index
     * @param item
     */
    void clear() {
        this->ptr->_version++;
        if constexpr (il2cpp_utils::il2cpp_reference_type<T>) {
            int size = this->size();
            this->ptr->_size = 0;
            if (size > 0) {
                std::fill(this->begin(), this->begin() + this->size(), T());
                return;
            }
        } else {
            this->ptr->_size = 0;
        }
    }

    /**
     * @brief System.Collections.Generic::List<T>.Insert(int index, T item)
     *
     * https://learn.microsoft.com/en-us/dotnet/api/system.collections.generic.list-1.insert?view=net-8.0
     * @param index
     * @param item
     */
    void insert_at(int index, T item) {
        if (index > this->size()) {
            throw std::runtime_error("Capacity size too small");
        }
        if (this->size() == this->get_items().size()) {
            this->EnsureCapacity(this->size() + 1);
        }
        if (index < this->size()) {
            std::copy(this->begin() + index, this->begin() + index + 1, this->begin() + (this->size() - index));
            // Array.Copy(this._items, index, this._items, index + 1,
            //            this._size - index);
        }
        this->ptr->_items[index] = item;
        this->ptr->_size++;
        this->ptr->_version++;
    }
    /**
     * @brief System.Collections.Generic::List<T>.Add(T item)
     * https://learn.microsoft.com/en-us/dotnet/api/system.collections.generic.list-1.add?view=net-8.0
     *
     * @param item
     */
    void push_back(T item) {
        this->ptr->_version++;
        auto items = this->get_items();
        auto size = this->size();
        if (size < items.size()) {
            this->ptr->_size = size + 1;
            items[size] = item;
            return;
        }
        this->AddWithResize(item);
    }

    template <typename... TArgs>
    requires(il2cpp_utils::il2cpp_reference_type<T>)
    void emplace_back(TArgs&&... args) {
        this->push_back(T::New_ctor(std::forward<TArgs>(args)...));
    }

    template <typename... TArgs>
    void emplace_back(TArgs&&... args) {
        this->as_il2cpp_list()->Add(T(std::forward<TArgs>(args)...));
    }

    ArrayW<T> to_array() {
        ArrayW<T> newArr = ArrayW<T>(this->_size);
        std::copy(this->_items.begin(), this->_items.begin() + this->size(), newArr.begin());

        return newArr;
    }

    /**
     * @brief System.Collections.Generic::List<T>.Remove(T item)
     *
     * https://learn.microsoft.com/en-us/dotnet/api/system.collections.generic.list-1.remove?view=net-8.0
     * @param index
     * @param item
     */
    bool erase(T item) {
        auto index = index_of(item);
        if (!index.has_value()) {
            return false;
        }

        erase_at(index.value());
        return true;
    }

    /**
     * @brief System.Collections.Generic::List<T>.RemoveAt(int index)
     *
     * https://learn.microsoft.com/en-us/dotnet/api/system.collections.generic.list-1.removeat?view=net-8.0
     * @param index
     * @param item
     */
    void erase_at(int index) {
        if (index >= this->size()) {
            throw std::runtime_error("index is over size bounds");
        }
        this->ptr->_size--;
        if (index < this->size()) {
            std::copy(this->begin() + 1, this->end(), this->begin() + this->size() - index);
            // Array.Copy(this._items, index + 1, this._items, index, this._size - index);
        }
        if constexpr (il2cpp_utils::il2cpp_reference_type<T>) {
            this->_items[this->size()] = T();
        }
        this->ptr->_version++;
    }

    /**
     * @brief System.Collections.Generic::List<T>.RemoveRange(int index, int count)
     *
     * https://learn.microsoft.com/en-us/dotnet/api/system.collections.generic.list-1.removerange?view=net-8.0
     * @param index
     * @param item
     */
    void erase_range(int index, int count) {
        if (index < 0) {
            throw std::runtime_error("index is less than 0");
        }
        if (count < 0) {
            throw std::runtime_error("count is less than 0");
        }
        if (this->size() - index < count) {
            throw std::runtime_error("count is over bounds");
        }
        if (count <= 0) {
            return;
        }
        // int size = this->size();
        this->_size -= count;
        if (index < this->size()) {
            std::copy(this->begin() + index + count, this->begin() + (this->size() - index), this->begin() + index);
            // Array.Copy(this._items, index + count, this._items, index, this._size - index);
        }
        this->_version++;
        if constexpr (il2cpp_utils::il2cpp_reference_type<T>) {
            std::fill(this->_items.begin() + this->size(), this->items.begin() + this->size() + count, T());
            // Array.Clear(this._items, this._size, count);
        }
    }

    /**
     * @brief Adds the collection to the end of the List. Ensures capacity is appropiate
     *
     * System.Collections.Generic::List<T>.AddRange(Enumerable<T> enumerable)
     * https://learn.microsoft.com/en-us/dotnet/api/system.collections.generic.list-1.addrange?view=net-8.0
     *
     * @tparam It Iterator type
     * @param begin
     * @param end
     */
    template <typename It>
    void insert_range(It begin, It end) {
        insert_span(std::span<T>(begin, end));
    }

    /**
     * @brief Adds the collection to the end of the List. Ensures capacity is appropiate
     *
     * System.Collections.Generic::List<T>.AddRange(Enumerable<T> enumerable)
     * https://learn.microsoft.com/en-us/dotnet/api/system.collections.generic.list-1.addrange?view=net-8.0
     *
     * @tparam It
     * @param begin
     * @param count amount of items
     */
    template <typename It>
    void insert_range(It begin, int count) {
        insert_span(std::span<T>(begin, count));
    }

    /**
     * @brief Adds the collection to the end of the List. Ensures capacity is appropiate
     *
     * System.Collections.Generic::List<T>.AddRange(Enumerable<T> enumerable)
     * https://learn.microsoft.com/en-us/dotnet/api/system.collections.generic.list-1.addrange?view=net-8.0
     *
     * @tparam C
     * @param container
     */
    template <typename C>
    void insert_range(C container) {
        insert_span(std::span<T>(container.begin(), container.end()));
    }

    /**
     * @brief Adds the collection to the end of the List. Ensures capacity is appropiate
     *
     * System.Collections.Generic::List<T>.AddRange(Enumerable<T> enumerable)
     * https://learn.microsoft.com/en-us/dotnet/api/system.collections.generic.list-1.addrange?view=net-8.0
     *
     * @param span
     */
    void insert_span(std::span<T> span) {
        if (span.empty()) return;

        this->EnsureCapacity(span.size() + this->size());
        auto start = this->begin() + this->size();
        std::copy(span.begin(), span.end(), start);

        ptr->_size += span.size();
        ptr->_version++;
    }

    /// @brief Provides a reference span of the held data within this array. The span should NOT outlive this instance.
    /// @return The created span.
    std::span<T> ref_to() {
        return std::span(begin(), end());
    }

    /// @brief Provides a reference span of the held data within this array. The span should NOT outlive this instance.
    /// @return The created span.
    const std::span<T> ref_to() const {
        return std::span(begin()());
    }

   private:
    auto const& get_items() const {
        return ptr->_items;
    }
    auto& get_items() {
        return ptr->_items;
    }

    auto as_il2cpp_list() {
        return reinterpret_cast<List<T>*>(ptr);
    }

   protected:
    void SetCapacity(int value) {
        if (value < this->size()) {
            throw std::runtime_error("Capacity size too small");
        }
        if (value != this->get_items().size()) {
            if (value > 0) {
                auto array = ArrayW<T>(value);
                if (this->size() > 0) {
                    std::copy(this->begin(), this->end(), array.begin());
                }
                ptr->_items = array;
                return;
            }
            ptr->_items = ArrayW<T>(il2cpp_array_size_t(0));
        }
    }

    void AddWithResize(T item) {
        auto size = this->size();
        this->EnsureCapacity(size + 1);
        ptr->_size = size + 1;
        get_items()[size] = item;
    }

    void EnsureCapacity(int min) {
        if (get_items().size() < min) {
            int num = (get_items().size() == 0) ? 4 : (get_items().size() * 2);
            if (num > 2146435071) {
                num = 2146435071;
            }
            if (num < min) {
                num = min;
            }
            SetCapacity(num);
        }
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
