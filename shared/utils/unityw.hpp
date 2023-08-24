#pragma once
#include "base-wrapper-type.hpp"

namespace bs_hook {
    class UnityW : public Il2CppWrapperType {
        public:
        constexpr UnityW(void* o) : Il2CppWrapperType(o) {}
    };
}
