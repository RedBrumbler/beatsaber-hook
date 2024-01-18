#ifdef TEST_UNITYW

#include <iostream>
#include "../../shared/utils/il2cpp-utils.hpp"
#include "../../shared/utils/typedefs.h"

static void constDoThing(const UnityW<Il2CppObject> wrap) {
    [[maybe_unused]] bool x = wrap.isAlive();
    [[maybe_unused]] void* y = wrap.convert();
    [[maybe_unused]] Il2CppObject const* z = wrap.ptr();
    [[maybe_unused]] Il2CppObject const* w = wrap.unsafePtr();
}

static void doThing() {
    UnityW<Il2CppObject> wrap;
    [[maybe_unused]] bool x = wrap.isAlive();
    [[maybe_unused]] void* y = wrap.convert();

    [[maybe_unused]] Il2CppObject* z = wrap.ptr();
    [[maybe_unused]] Il2CppObject* w = wrap.unsafePtr();
}

#endif
