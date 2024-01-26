#pragma once

#include <concepts>
#include "il2cpp-type-check.hpp"
#include "type-concepts.hpp"
#include "il2cpp-utils-exceptions.hpp"
#include "typedefs-array.hpp"

template<typename TKey, typename TValue>
struct Dictionary;
template<typename TKey, typename TValue>
struct Entry;
template<typename TKey, typename TValue>
struct KeyValuePair;

template<typename TKey, typename TValue, typename Ptr>
struct DictionaryWrapper;

/// @brief Represents an exception thrown from usage of a Dictionary
struct DictionaryException : il2cpp_utils::exceptions::StackTraceException {
    void* dictionaryInstance;
    DictionaryException(void* instance, std::string_view msg) : il2cpp_utils::exceptions::StackTraceException(msg.data()), dictionaryInstance(instance) {}
};

template<typename TKey, typename TValue>
struct KeyValuePair {
    TKey key;
    TValue value;
};

DEFINE_IL2CPP_ARG_TYPE_GENERIC_STRUCT(KeyValuePair, "System.Collections.Generic", "KeyValuePair`2");

template<typename TKey, typename TValue>
struct Entry {
    int hashCode;
    int next;
    TKey key;
    TValue value;
};

static_assert(sizeof(Entry<int, int>) == 0x10);
DEFINE_IL2CPP_ARG_TYPE_GENERIC_STRUCT(Entry, "System.Collections.Generic", "Dictionary`2/Entry");

template<typename TKey, typename TValue>
struct Dictionary : public Il2CppObject {
    using Entry = ::Entry<TKey, TValue>;

    ArrayW<int> _buckets;
    ArrayW<Entry> _entries;
    int _count;
    int _freeList;
    int _freeCount;
    int _version;
    Il2CppObject* _comparer;
    Il2CppObject* _keys;
    Il2CppObject* _values;
    Il2CppObject* _syncRoot;

    static Dictionary<TKey, TValue>* New() {
        auto dict = il2cpp_utils::New<Dictionary<TKey, TValue>*>();
        #if __has_feature(cxx_exceptions)
            if (!dict) throw DictionaryException(nullptr, "Was unable to allocate dictionary!");
        #else
            if (!dict) SAFE_ABORT_MSG("Was unable to allocate dictionary!");
        #endif
        return dict;
    }

    void Add(KeyValuePair<TKey, TValue> kvp) {
        Add(kvp.key, kvp.value);
    }

    void Add(TKey key, TValue value) {

    }
};

// size check
static_assert(sizeof(Dictionary<int, int>) == 0x50);
DEFINE_IL2CPP_ARG_TYPE_GENERIC_CLASS(Dictionary, "System.Collections.Generic", "Dictionary`2");

template<typename TKey, typename TValue, typename Ptr = Dictionary<TKey, TValue>*>
struct DictionaryWrapper {
    static_assert(sizeof(Ptr) == sizeof(void*), "Size of Ptr must be equal to void*!");

    constexpr DictionaryWrapper() noexcept : val(nullptr) {}
    constexpr DictionaryWrapper(nullptr_t) noexcept : val(nullptr) {}

    // il2cpp conversion
    constexpr explicit DictionaryWrapper(void* i) noexcept : val(i) {}
    constexpr void* convert() const noexcept { return const_cast<void*>(static_cast<const void*>(val)); }

    TValue get(TKey key) const {
        auto num = FindEntry(key);

        if (num >= 0) {
            return val->_entries[num].value;
        }

        throw DictionaryException(val, "Could not find key in dictionary!");
    }

    void set(TKey key, TValue value) {

    }

    // TODO: Some kind of wrapper return to assign / read from the dictionary using operator=?
    TValue operator[](TKey key) const {
        return get(key);
    }

    int FindEntry(TKey key) const {
        if constexpr (std::equality_comparable_with<TKey, nullptr_t>) {
            if (key == nullptr) throw DictionaryException(val, "Key failed to convert to true boolean, equivalent to failing a null check!");
        }

        int num = -1;
        int writes = 0;
        auto buckets = val->_buckets;
        auto entries = val->_entries;

        if (buckets) {
            auto comparer = val->_comparer;
            if (!comparer) { // no comparer, use default comparison
                static auto hashcode_methodinfo = THROW_UNLESS(
                    il2cpp_utils::FindMethod(
                        classof(TKey),
                        "GetHashCode",
                        std::array<const Il2CppClass*, 0>{},
                        std::array<const Il2CppType*, 0>{}
                    )
                );

                auto hash = il2cpp_utils::RunMethodRethrow<int>(key, hashcode_methodinfo) & 0xffffffffffffffffu;
                num = buckets[hash % buckets.size()] - 1;

                // this checks for when entries collide and finds the next suitable index to use
                while (num < entries.size() && (entries[num].hashCode != hash || entries[num].key != key)) {
                    num = entries[num].next;
                    if (writes >= entries.size()) throw DictionaryException(val, "Concurrent operation happened! this is not supported!");
                    writes++;
                }
            } else { // comparer is set, use it
                auto hash = il2cpp_utils::RunMethod<int>(comparer, "GetHashCode", key).get_result() & 0xffffffffffffffffu;
                num = buckets[hash % buckets.size()] - 1;

                auto equals_methodinfo = THROW_UNLESS(
                    il2cpp_utils::FindMethod(
                        il2cpp_functions::object_get_class(comparer),
                        "Equals",
                        std::array<const Il2CppClass*, 0>{},
                        std::array<const Il2CppType*, 2>{classof(TKey)->byval_arg, classof(TKey)->byval_arg}
                    )
                );

                // this checks for when entries collide and finds the next suitable index to use
                while (num < entries.size() && (entries[num].hashCode != hash || il2cpp_utils::RunMethodRethrow<bool>(comparer, equals_methodinfo, entries[num].key, key))) {
                    num = entries[num].next;
                    if (writes >= entries.size()) throw DictionaryException(val, "Concurrent operation happened! this is not supported!");
                    writes++;
                }
            }
        }

        return num;
    }

    private:
        Ptr val;
};

static_assert(il2cpp_utils::has_il2cpp_conversion<DictionaryWrapper<int, int>>);

MARK_GEN_REF_T(DictionaryWrapper);
MARK_GEN_REF_PTR_T(Dictionary);

template<typename TKey, typename TValue, typename Ptr>
struct il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<DictionaryWrapper<TKey, TValue, Ptr>> {
    static Il2CppClass* get() {
        return classof(Ptr);
    }
};
