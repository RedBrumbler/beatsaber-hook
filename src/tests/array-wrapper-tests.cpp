#ifdef TEST_ARRAY

#include "../../shared/utils/typedefs.h"
#include <iostream>
#include <cassert>

static void constDoThing(const ArrayW<int>& wrap) {
    auto i = wrap[0];
    assert(wrap.Length() == 5);
    for (auto itr : wrap) {
        // do thing with each int, const
        assert(itr == i);
        std::cout << itr << std::endl;
    }
    std::cout << i << std::endl;
}

ArrayW<float> initThing;

static bool search(int&) {
    return false;
}

static void doThing() {
    ArrayW<int> arr(5);
    ArrayW<int> arr2(arr);
    // Init the pointer to nullptr
    ArrayW<int*> arr3(nullptr);
    auto i = arr[0];
    assert(arr.Length() == 5);
    assert(arr2.Length() == 5);
    for (auto itr : arr) {
        // do thing with each int
        assert(itr == i);
        std::cout << itr << std::endl;
    }
    for (auto itr : arr2) {
        // do thing with each int
        assert(itr == i);
        std::cout << itr << std::endl;
    }
    std::cout << arr.size() << std::endl;
    std::cout << static_cast<Array<int*>*>(arr3) << std::endl;
    // Should be allowed to cast back
    std::cout << static_cast<Array<int>*>(arr) << std::endl;
    std::cout << i << std::endl;
    // Should be simply nullptr
    std::cout << static_cast<Array<float>*>(initThing) << std::endl;

    /// get first element that fulfills the predicate
    arr.front_or_default();
    arr3.front();
    arr.front_or_default([](auto x){ return x == 0; });
    arr3.front([](auto x){ return x == 0; });
    arr.find(5);
    arr3.find_if([](auto x){ return x == 0; });

    auto search_fun = [](int&){ return false; };
    // test passing in saved lambda
    arr.find_if(search_fun);
    // test passing in function ptr
    arr.find_if(search);

    /// get first reverse iter element that fulfills the predicate
    arr.back_or_default();
    arr3.back();
    arr.back_or_default([](auto x){ return x == 0; });
    arr3.back([](auto x){ return x == 0; });
    int v;
    arr3.rfind(&v);
    arr3.rfind_if([](auto x){ return x == 0; });
}

#include "../../shared/utils/il2cpp-utils.hpp"

static void doThing2() {
    ArrayW<int> arr(2);
    MethodInfo info;
    il2cpp_utils::RunMethodThrow(static_cast<Il2CppObject*>(nullptr), &info, arr);
    il2cpp_utils::RunMethodThrow<ArrayW<Il2CppObject*>>(static_cast<Il2CppObject*>(nullptr), &info);
    il2cpp_utils::RunMethod<ArrayW<Il2CppObject*>>(static_cast<Il2CppObject*>(nullptr), &info);
    if (arr) {
        ArrayW<float> x(static_cast<ArrayW<float>>(arr));
        for (auto itr : x) {
            std::cout << itr;
        }
    }
    auto emptyArr = ArrayW<int>::Empty();
    for (auto& v : emptyArr) {
        v = 0;
    }
    il2cpp_utils::RunMethod(arr, "test", arr);
    il2cpp_utils::RunMethodRethrow<ArrayW<Il2CppObject*>>((Il2CppClass*)nullptr, &info);
}

#endif
