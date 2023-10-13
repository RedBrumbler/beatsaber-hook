#pragma once
#include "typedefs-object.hpp"

namespace bs_hook {
    class UnityW : public ::ObjectWrapperType {
        public:
        constexpr UnityW(void* o) : ObjectWrapperType(o) {}
    };
}
