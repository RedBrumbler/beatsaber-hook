#pragma once

#include "utils-functions.h"
#include "il2cpp-type-check.hpp"
#include "il2cpp-utils-exceptions.hpp"
#include <string_view>
#include <string>
#include <stdexcept>
#include <locale>
#include <codecvt>
#include <span>

struct UseBeforeInitError : il2cpp_utils::exceptions::StackTraceException {
    UseBeforeInitError(const char* v) : il2cpp_utils::exceptions::StackTraceException(v) {}
};

struct Il2CppString;

namespace il2cpp_utils {
namespace detail {
    void convstr(char const* inp, char16_t* outp, int sz);
    std::size_t convstr(char16_t const* inp, char* outp, int isz, int osz);

    Il2CppString* alloc_str(std::string_view str);
    Il2CppString* alloc_str(std::u16string_view str);

    Il2CppString* strappend(Il2CppString const* lhs, Il2CppString const* rhs) noexcept;
    Il2CppString* strappend(Il2CppString const* lhs, std::u16string_view const rhs) noexcept;
    Il2CppString* strappend(Il2CppString const* lhs, std::string_view const rhs) noexcept;
    Il2CppString* strappend(std::string_view const lhs, Il2CppString const* rhs) noexcept;
    Il2CppString* strappend(std::u16string_view const lhs, Il2CppString const* rhs) noexcept;

    bool strcomp(Il2CppString const* lhs, std::string_view const rhs) noexcept;
    bool strcomp(Il2CppString const* lhs, std::u16string_view const rhs) noexcept;
    bool strcomp(Il2CppString const* lhs, Il2CppString const* rhs) noexcept;

    bool strless(Il2CppString const* lhs, std::string_view const rhs) noexcept;
    bool strless(Il2CppString const* lhs, std::u16string_view const rhs) noexcept;
    bool strless(Il2CppString const* lhs, Il2CppString const* rhs) noexcept;
    
    bool strstart(Il2CppString const* lhs, std::string_view const rhs) noexcept;
    bool strstart(Il2CppString const* lhs, std::u16string_view const rhs) noexcept;
    bool strstart(Il2CppString const* lhs, Il2CppString const* rhs) noexcept;

    bool strend(Il2CppString const* lhs, std::string_view const rhs) noexcept;
    bool strend(Il2CppString const* lhs, std::u16string_view const rhs) noexcept;
    bool strend(Il2CppString const* lhs, Il2CppString const* rhs) noexcept;
}
}

// C# strings can only have 'int' max length.
template<int sz>
struct ConstString {
    // Manually allocated string, dtor destructs in place
    ConstString(const char (&st)[sz]) {
        if (il2cpp_functions::initialized) {
            klass = il2cpp_functions::defaults->string_class;
        }
        length = sz - 1;
        il2cpp_utils::detail::convstr(st, chars, sz - 1);
    }
    constexpr ConstString(const char16_t (&st)[sz]) noexcept {
        length = sz - 1;
        for (int i = 0; i < sz - 1; i++) {
            chars[i] = st[i];
        }
    }
    // Copies allowed? But should probably be avoided.
    ConstString(ConstString const&) = default;
    // Moves allowed
    ConstString(ConstString&&) = default;

    void init() noexcept {
        klass = il2cpp_functions::defaults->string_class;
    }

    constexpr operator Il2CppString*() {
        if (!klass) {
            if (il2cpp_functions::initialized) {
                klass = il2cpp_functions::defaults->string_class;
            } else {
                throw UseBeforeInitError("Il2CppClass* must be initialized before conversion! Call il2cpp_functions::Init before this conversion!");
            }
        }
        return reinterpret_cast<Il2CppString*>(&klass);
    }

    constexpr operator Il2CppString const*() const {
        if (!klass) {
            if (il2cpp_functions::initialized) {
                // due to klass being initialized being essential to the functionality of this type, we agreed that ignoring the const here is warranted
                // usually const casting is bad, but due to the reasons stated above we are doing it anyways
                const_cast<ConstString<sz>*>(this)->klass = il2cpp_functions::defaults->string_class;
            } else {
                throw UseBeforeInitError("Il2CppClass* must be initialized before conversion! Call il2cpp_functions::Init before this conversion!");
            }
        }
        return reinterpret_cast<Il2CppString const*>(&klass);
    }

    constexpr Il2CppString* operator->() {
        return operator Il2CppString*();
    }

    operator std::string() {
        std::string val((sz - 1) * 2 + 1, '\0');
        auto resSize = il2cpp_utils::detail::convstr(chars, val.data(), sz - 1, val.size());
        val.resize(resSize);
        return val;
    }
    operator std::u16string() {
        return {chars, chars + length};
    }
    operator std::wstring() {
        return {chars, chars + length};
    }
    constexpr operator std::u16string_view() {
        return {chars, static_cast<std::size_t>(sz)};
    }

    friend struct StringW;
    private:
    void* klass = nullptr;
    void* monitor;
    int length;
    char16_t chars[sz];
};

struct StringW {
    // Dynamically allocated string
    template<class T>
    requires (!std::is_convertible_v<T, Il2CppString*> && (std::is_constructible_v<std::u16string_view, T> || std::is_constructible_v<std::string_view, T>))
    StringW(T str) noexcept : inst(il2cpp_utils::detail::alloc_str(str)) {}
    constexpr StringW(void* ins) noexcept : inst(static_cast<Il2CppString*>(ins)) {}
    constexpr StringW(Il2CppString* ins) noexcept : inst(ins) {}
    template <int sz>
    constexpr StringW(ConstString<sz>& conststring) noexcept : inst(static_cast<Il2CppString*>(conststring)) {}
    constexpr StringW(std::nullptr_t npt) noexcept : inst(npt) {}
    constexpr StringW() noexcept : inst(nullptr) {}

    constexpr void const* convert() const noexcept {
        return inst;
    }
    constexpr void* convert() noexcept {
        return inst;
    }
    constexpr operator Il2CppString const*() const noexcept {
        return inst;
    }
    constexpr operator Il2CppString*() noexcept {
        return inst;
    }
    constexpr Il2CppString const* operator->() const noexcept {
        return inst;
    }
    constexpr Il2CppString* operator->() noexcept {
        return inst;
    }
    constexpr operator bool() const noexcept {
        return inst != nullptr; 
    }

    constexpr bool operator ==(std::nullptr_t rhs) const noexcept {
        return inst == rhs;
    }

    template<typename T>
    requires (std::is_constructible_v<std::u16string_view, T> || std::is_constructible_v<std::string_view, T> || std::is_same_v<T, StringW>)
    StringW& operator +=(T const& rhs) noexcept {
        if constexpr (std::is_same_v<T, StringW>) inst = il2cpp_utils::detail::strappend(inst, rhs.inst);
        else inst = il2cpp_utils::detail::strappend(inst, rhs);
        return *this;
    }

    template<typename T>
    requires (std::is_constructible_v<std::u16string_view, T> || std::is_constructible_v<std::string_view, T> || std::is_same_v<T, StringW>)
    StringW operator +(T const& rhs) const noexcept {
        if constexpr (std::is_same_v<T, StringW>) return il2cpp_utils::detail::strappend(inst, rhs.inst);
        else return il2cpp_utils::detail::strappend(inst, rhs);
    }

    template<typename T>
    requires (std::is_constructible_v<std::u16string_view, T> || std::is_constructible_v<std::string_view, T> || std::is_same_v<T, StringW>)
    bool operator <(T const& rhs) const noexcept {
        if constexpr (std::is_same_v<T, StringW>) return il2cpp_utils::detail::strless(inst, rhs.inst);
        else return il2cpp_utils::detail::strless(inst, rhs);
    }

    template<int sz>
    bool operator ==(ConstString<sz> const& rhs) const noexcept {
        return il2cpp_utils::detail::strcomp(inst, rhs.operator Il2CppString const*());
    }
    
    template<typename T>
    requires (std::is_constructible_v<std::u16string_view, T> || std::is_constructible_v<std::string_view, T> || std::is_same_v<T, StringW>)
    bool operator ==(T const& rhs) const noexcept {
        if constexpr (std::is_same_v<T, StringW>) return il2cpp_utils::detail::strcomp(inst, rhs.inst);
        else return il2cpp_utils::detail::strcomp(inst, rhs);
    }

    template<typename T>
    requires (std::is_constructible_v<std::u16string_view, T> || std::is_constructible_v<std::string_view, T> || std::is_same_v<T, StringW>)
    bool starts_with(T const& rhs) const noexcept {
        if constexpr (std::is_same_v<T, StringW>) return il2cpp_utils::detail::strstart(inst, rhs.inst);
        else return il2cpp_utils::detail::strstart(inst, rhs);
    }
    
    template<typename T>
    requires (std::is_constructible_v<std::u16string_view, T> || std::is_constructible_v<std::string_view, T> || std::is_same_v<T, StringW>)
    bool ends_with(T const& rhs) const noexcept {
        if constexpr (std::is_same_v<T, StringW>) return il2cpp_utils::detail::strend(inst, rhs.inst);
        else return il2cpp_utils::detail::strend(inst, rhs);
    }
    
    using iterator = Il2CppChar*;
    using const_iterator = Il2CppChar const*;

    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
    operator std::span<Il2CppChar>();
    operator std::span<Il2CppChar const> const () const;

    Il2CppChar const& operator [](size_t const& idx) const;
    Il2CppChar& operator [](size_t const& idx);
    operator std::string() const;
    operator std::u16string() const;
    operator std::wstring() const;
    operator std::u16string_view();
    operator std::u16string_view const() const;

    private:
    Il2CppString* inst;
};

template<typename T>
requires (!std::is_constructible_v<T, StringW> && (std::is_constructible_v<std::u16string_view, T> || std::is_constructible_v<std::string_view, T>))
StringW operator +(T const lhs, StringW const& rhs) noexcept {
    return il2cpp_utils::detail::strappend(lhs, rhs.operator Il2CppString const*());
}

static_assert(sizeof(StringW) == sizeof(void*));
DEFINE_IL2CPP_DEFAULT_TYPE(StringW, string);
NEED_NO_BOX(StringW);
