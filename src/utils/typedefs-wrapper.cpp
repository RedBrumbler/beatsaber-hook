#include <string.h>
#include <locale>
#include <type_traits>
#include "../../shared/utils/il2cpp-functions.hpp"
#include "../../shared/utils/typedefs-string.hpp"
#include "../../shared/utils/typedefs-wrappers.hpp"
#include "../../shared/utils/typedefs.h"

std::unordered_map<void*, size_t> Counter::addrRefCount;
std::shared_mutex Counter::mutex;

namespace il2cpp_utils {
namespace detail {
template <class Facet>
struct deletable_facet : Facet {
    template <class... Args>
    deletable_facet(Args&&... args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

// Note that char is actually required here over char8_t-- this is due to NDK not having a char8_t specialization for this yet.
deletable_facet<std::codecvt<char16_t, char8_t, std::mbstate_t>> conv;

template <class T>
concept is_specialized = std::is_same_v<char16_t, typename T::intern_type>;
static_assert(is_specialized<std::codecvt<char16_t, char8_t, std::mbstate_t>>);

void convstr(char const* inp, char16_t* outp, int sz) {
    std::mbstate_t state;
    char8_t const* from_next;
    char16_t* to_next;
    conv.in(state, reinterpret_cast<char8_t const*>(inp), reinterpret_cast<char8_t const*>(inp + sz), from_next, outp, outp + sz, to_next);
}
std::size_t convstr(char16_t const* inp, char* outp, int isz, int osz) {
    std::mbstate_t state;
    char16_t const* from_next;
    char8_t* to_next;
    auto convOut = conv.out(state, inp, inp + isz, from_next, reinterpret_cast<char8_t*>(outp), reinterpret_cast<char8_t*>(outp + osz), to_next);
    if (convOut != std::codecvt_base::ok) {
        throw convOut;
    }
    return (std::size_t)(to_next - reinterpret_cast<char8_t*>(outp));
}

Il2CppString* alloc_str(std::string_view str) {
    il2cpp_functions::Init();

    if (str.data() == nullptr) {
        return il2cpp_functions::string_new_len("", 0);
    }

    return il2cpp_functions::string_new_len(str.data(), str.size());
}
Il2CppString* alloc_str(std::u16string_view str) {
    il2cpp_functions::Init();
    if (str.data() == nullptr) {
        return il2cpp_functions::string_new_len("", 0);
    }

    return il2cpp_functions::string_new_utf16((Il2CppChar const*)str.data(), str.size());
}

Il2CppString* CreateString(int length) {
    static MethodInfo const* methodInfo = il2cpp_utils::FindMethod(classof(Il2CppString*), "CreateString",
                                                                   std::array<Il2CppType const*, 2>{ il2cpp_utils::ExtractIndependentType<Il2CppChar>(), il2cpp_utils::ExtractIndependentType<int>() });
    return CRASH_UNLESS(il2cpp_utils::RunStaticMethodUnsafe<Il2CppString*>(methodInfo, Il2CppChar('\0'), length));
}

Il2CppString* strappend(Il2CppString const* lhs, Il2CppString const* rhs) noexcept {
    if (!lhs && !rhs) return nullptr;

    il2cpp_functions::Init();
    if (!lhs && rhs)
        return il2cpp_functions::string_new_utf16(rhs->chars, rhs->length);
    else if (lhs && !rhs)
        return il2cpp_functions::string_new_utf16(lhs->chars, lhs->length);
    else {
        size_t fullLength = lhs->length + rhs->length;
        Il2CppString* result = CreateString(fullLength);
        memcpy(result->chars, lhs->chars, lhs->length * sizeof(Il2CppChar));
        Il2CppChar* pastFirstString = result->chars + lhs->length;
        memcpy(pastFirstString, rhs->chars, rhs->length * sizeof(*rhs->chars));
        return result;
    }
}

Il2CppString* strappend(Il2CppString const* lhs, std::u16string_view const rhs) noexcept {
    if (lhs) {
        int fullLength = lhs->length + rhs.size();
        Il2CppString* result = CreateString(fullLength);
        memcpy(result->chars, lhs->chars, lhs->length * sizeof(Il2CppChar));
        Il2CppChar* pastFirstString = result->chars + lhs->length;
        static_assert(sizeof(*pastFirstString) == sizeof(*rhs.data()));
        memcpy(pastFirstString, rhs.data(), rhs.size() * sizeof(*rhs.data()));
        return result;
    } else {
        return alloc_str(rhs);
    }
}

Il2CppString* strappend(Il2CppString const* lhs, std::string_view const rhs) noexcept {
    if (lhs) {
        int fullLength = lhs->length + rhs.size();
        Il2CppString* result = CreateString(fullLength);
        memcpy(result->chars, lhs->chars, lhs->length * sizeof(Il2CppChar));
        Il2CppChar* pastFirstString = result->chars + lhs->length;
        convstr(rhs.data(), pastFirstString, rhs.size());
        return result;
    } else {
        return alloc_str(rhs);
    }
}

Il2CppString* strappend(std::string_view const lhs, Il2CppString const* rhs) noexcept {
    if (rhs) {
        int fullLength = rhs->length + lhs.size();
        Il2CppString* result = CreateString(fullLength);
        convstr(lhs.data(), result->chars, lhs.size());
        Il2CppChar* pastFirstString = result->chars + lhs.size();
        memcpy(pastFirstString, rhs->chars, rhs->length * sizeof(*pastFirstString));
        return result;
    } else {
        return alloc_str(lhs);
    }
}

Il2CppString* strappend(std::u16string_view const lhs, Il2CppString const* rhs) noexcept {
    if (rhs) {
        int fullLength = rhs->length + lhs.size();
        Il2CppString* result = CreateString(fullLength);
        static_assert(sizeof(Il2CppChar) == sizeof(*lhs.data()));
        memcpy(result->chars, lhs.data(), lhs.size() * sizeof(*lhs.data()));
        Il2CppChar* pastFirstString = result->chars + lhs.size();
        memcpy(pastFirstString, rhs->chars, rhs->length * sizeof(*pastFirstString));
        return result;
    } else {
        return alloc_str(lhs);
    }
}

template <typename T>
    requires(std::is_same_v<T, std::string_view> || std::is_same_v<T, std::u16string_view>)
bool strcomp(Il2CppString const* lhs, T const rhs) noexcept {
    if (!lhs || lhs->length != (int)rhs.size()) return false;

    Il2CppChar const* first = lhs->chars;
    auto const* second = rhs.data();
    Il2CppChar const* firstEnd = first + lhs->length;
    auto const* secondEnd = second + (int)rhs.size();

    while (first != firstEnd && second != secondEnd) {
        if (*first != *second) return false;
        first++;
        second++;
    }

    return first == firstEnd && second == secondEnd;
}

bool strcomp(Il2CppString const* lhs, std::string_view const rhs) noexcept {
    return strcomp<std::remove_const_t<decltype(rhs)>>(lhs, rhs);
}

bool strcomp(Il2CppString const* lhs, std::u16string_view const rhs) noexcept {
    return strcomp<std::remove_const_t<decltype(rhs)>>(lhs, rhs);
}

bool strcomp(Il2CppString const* lhs, Il2CppString const* rhs) noexcept {
    if (lhs == rhs) return true;
    if (!lhs || !rhs || lhs->length != rhs->length) return false;

    Il2CppChar const* first = lhs->chars;
    Il2CppChar const* second = rhs->chars;
    Il2CppChar const* firstEnd = first + lhs->length;
    Il2CppChar const* secondEnd = second + rhs->length;

    while (first != firstEnd && second != secondEnd) {
        if (*first != *second) return false;
        first++;
        second++;
    }

    return first == firstEnd && second == secondEnd;
}

template <typename T>
    requires(std::is_same_v<T, std::string_view> || std::is_same_v<T, std::u16string_view>)
    bool strless(Il2CppString const* lhs, T const rhs) noexcept {
        if (!lhs) return true;

        Il2CppChar const* first = lhs->chars;
        auto const* second = rhs.data();
        Il2CppChar const* firstEnd = lhs->chars + lhs->length;
        auto const* secondEnd = rhs.data() + rhs.size();

        while (first != firstEnd && second != secondEnd)
        {
            if (*first == *second)
            {
                first++; second++;
                continue;
            }
            return *first < *second;
        }
        return *first < *second;
    }
    // if we got here, and second is not second end, we had a shorter first, so it should be true
    // if second is the end, we are longer, so it should be false
    return second != secondEnd;
}

    bool strless(Il2CppString const* lhs, std::string_view const rhs) noexcept {
        return strless<std::remove_const_t<decltype(rhs)>>(lhs, rhs);
    }

    bool strless(Il2CppString const* lhs, std::u16string_view const rhs) noexcept {
        return strless<std::remove_const_t<decltype(rhs)>>(lhs, rhs);
    }

    bool strless(Il2CppString const* lhs, Il2CppString const* rhs) noexcept {
        if (!lhs && !rhs) return false;
        if (!lhs) return true;
        if (!rhs) return false;

        Il2CppChar const* first = lhs->chars;
        Il2CppChar const* second = rhs->chars;
        Il2CppChar const* firstEnd = first + lhs->length;
        Il2CppChar const* secondEnd = second + rhs->length;

        while (first != firstEnd && second != secondEnd)
        {
            if (*first == *second)
            {
                first++; second++;
                continue;
            }
            return *first < *second;
        }
        return *first < *second;
    }
    // if we got here, and second is not second end, we had a shorter first, so it should be true
    // if second is the end, we are longer, so it should be false
    return second != secondEnd;
}

template <typename T>
    requires(std::is_same_v<T, std::string_view> || std::is_same_v<T, std::u16string_view>)
    bool strstart(Il2CppString const* lhs, T const rhs) noexcept {
        if (!lhs || lhs->length < (decltype(lhs->length))rhs.size()) return false;

        Il2CppChar const* first = lhs->chars;
        auto const* second = rhs.data();
        auto const* secondEnd = second + rhs.size();

        while (second != secondEnd)
        {
            if (*first == *second)
            {
                first++; second++;
                continue;
            }
            // we got a mismatch! return false;
            return false;
        }
        // we got a mismatch! return false;
        return false;
    }
    // if we got through the entire string it was all equal, return true
    return true;
}

    bool strstart(Il2CppString const* lhs, std::string_view const rhs) noexcept {
        return strstart<std::remove_const_t<decltype(rhs)>>(lhs, rhs);
    }

    bool strstart(Il2CppString const* lhs, std::u16string_view const rhs) noexcept {
        return strstart<std::remove_const_t<decltype(rhs)>>(lhs, rhs);
    }

    bool strstart(Il2CppString const* lhs, Il2CppString const* rhs) noexcept {
        // if either instance is nullptr, return false, if our length is smaller than prefix length, also return false
        if (!lhs || !rhs || lhs->length < rhs->length) return false;

        Il2CppChar const* first = lhs->chars;
        Il2CppChar const* second = rhs->chars;
        Il2CppChar const* secondEnd = second + lhs->length;

        while (second != secondEnd)
        {
            if (*first == *second)
            {
                first++; second++;
                continue;
            }
            // we got a mismatch! return false;
            return false;
        }
        // we got a mismatch! return false;
        return false;
    }
    // if we got through the entire string it was all equal, return true
    return true;
}

template <typename T>
    requires(std::is_same_v<T, std::string_view> || std::is_same_v<T, std::u16string_view>)
    bool strend(Il2CppString const* lhs, T const rhs) noexcept {
        if (!lhs || lhs->length < (decltype(lhs->length))rhs.size()) return false;

        Il2CppChar const* first = lhs->chars + lhs->length - 1;
        auto const* secondBegin = rhs.data() - 1;
        auto const* second = secondBegin + rhs.size();

        while (second != secondBegin)
        {
            if (*first == *second)
            {
                first--; second--;
                continue;
            }
            // we got a mismatch! return false;
            return false;
        }
        // we got a mismatch! return false;
        return false;
    }
    // if we got through the entire string it was all equal, return true
    return true;
}

bool strend(Il2CppString const* lhs, std::string_view const rhs) noexcept {
    return strend<std::remove_const_t<decltype(rhs)>>(lhs, rhs);
}

bool strend(Il2CppString const* lhs, std::u16string_view const rhs) noexcept {
    return strend<std::remove_const_t<decltype(rhs)>>(lhs, rhs);
}

bool strend(Il2CppString const* lhs, Il2CppString const* rhs) noexcept {
    if (!lhs || !rhs || lhs->length < rhs->length) return false;

    Il2CppChar const* first = lhs->chars + lhs->length - 1;
    Il2CppChar const* secondBegin = rhs->chars - 1;
    Il2CppChar const* second = secondBegin + rhs->length;

    while (second != secondBegin) {
        if (*first == *second) {
            first--;
            second--;
            continue;
        }
        // we got a mismatch! return false;
        return false;
    }
    // if we got through the entire string it was all equal, return true
    return true;
}

}  // namespace detail
}  // namespace il2cpp_utils

StringW::operator std::string() const {
    auto inst = get_inst();
    std::string val(inst->length * sizeof(wchar_t) + 1, '\0');
    auto resSize = il2cpp_utils::detail::convstr(inst->chars, val.data(), inst->length, val.size());
    val.resize(resSize);
    return val;
}

StringW::operator std::u16string() const {
    auto inst = get_inst();
    return {inst->chars, inst->chars + inst->length};
}

StringW::operator std::wstring() const {
    auto inst = get_inst();
    return {inst->chars, inst->chars + inst->length};
}

StringW::operator std::u16string_view const() const {
    auto inst = get_inst();
    return {inst->chars, static_cast<std::size_t>(inst->length)};
}

StringW::operator std::u16string_view() {
    auto inst = get_inst();
    return {inst->chars, static_cast<std::size_t>(inst->length)};
}

StringW::iterator StringW::begin() { return get_inst()->chars; }
StringW::const_iterator StringW::begin() const { return get_inst()->chars; }
StringW::iterator StringW::end() { return get_inst()->chars + get_inst()->length; }
StringW::const_iterator StringW::end() const { return get_inst()->chars + get_inst()->length; }
StringW::operator std::span<Il2CppChar>() {
    return { begin(), end() };
}
StringW::operator std::span<Il2CppChar const> const() const {
    return { begin(), end() };
}
Il2CppChar const& StringW::operator[](size_t const& idx) const {
    return inst->chars[idx];
}
Il2CppChar& StringW::operator[](size_t const& idx) {
    return inst->chars[idx];
}
Il2CppChar const& StringW::operator [](size_t const& idx) const { return get_inst()->chars[idx]; }
Il2CppChar& StringW::operator [](size_t const& idx) { return get_inst()->chars[idx]; }
