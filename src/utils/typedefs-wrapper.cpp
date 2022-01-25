#include "../../shared/utils/typedefs-wrappers.hpp"
#include "../../shared/utils/typedefs-string.hpp"
#include "../../shared/utils/il2cpp-functions.hpp"
#include "../../shared/utils/typedefs.h"
#include <locale>

std::unordered_map<void*, size_t> Counter::addrRefCount;
std::shared_mutex Counter::mutex;

namespace il2cpp_utils {
namespace detail {
    template<class Facet>
    struct deletable_facet : Facet {
        template<class ...Args>
        deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
        ~deletable_facet() {}
    };

    // Note that char is actually required here over char8_t-- this is due to NDK not having a char8_t specialization for this yet.
    deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>> conv;

    void convstr(char const* inp, char16_t* outp, int sz) {
        std::mbstate_t state;
        const char* from_next;
        char16_t* to_next;
        conv.in(state, inp, inp + sz, from_next, outp, outp + sz, to_next);
        outp[sz] = '\0';
    }
    void convstr(char16_t const* inp, char* outp, int sz) {
        std::mbstate_t state;
        const char16_t* from_next;
        char* to_next;
        conv.out(state, inp, inp + sz, from_next, outp, outp + sz, to_next);
        outp[sz] = '\0';
    }

    Il2CppString* alloc_str(std::string_view str) {
        il2cpp_functions::Init();
        return il2cpp_functions::string_new_len(str.data(), str.size());
    }
    Il2CppString* alloc_str(std::u16string_view str) {
        il2cpp_functions::Init();
        return il2cpp_functions::string_new_utf16((Il2CppChar*) str.data(), str.size());
    }
}
}

StringW::operator std::string() {
    std::string val;
    val.resize(inst->length, '\0');
    il2cpp_utils::detail::convstr(inst->chars, val.data(), inst->length);
    val.shrink_to_fit();
    return val;
}

StringW::operator std::u16string() {
    return {inst->chars, inst->chars + inst->length};
}

StringW::operator std::wstring() {
    return {inst->chars, inst->chars + inst->length};
}

StringW::operator std::u16string_view() {
    return {inst->chars, static_cast<std::size_t>(inst->length)};
}
