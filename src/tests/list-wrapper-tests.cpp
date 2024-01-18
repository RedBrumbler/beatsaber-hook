#ifdef TEST_LIST

#include "../../shared/utils/typedefs.h"
#include "../../shared/utils/il2cpp-utils.hpp"
#include <iostream>

static void constDoThing(const ListW<int>& wrap) {
    auto i = wrap[0];
    assert(wrap.size() == 1);
    for (auto itr : wrap) {
        // do thing with each int, const
        assert(itr == i);
        std::cout << itr << std::endl;
    }
    std::cout << i << std::endl;
}

static void doThing() {
    ListW<int> arr(*il2cpp_utils::New<List<int>*>(classof(List<int>*)));
    il2cpp_utils::RunMethodThrow(*reinterpret_cast<List<int>**>(&arr), il2cpp_utils::FindMethod(arr, "Add"), 2);
    auto i = arr[0];
    assert(arr.size() == 1);
    for (auto itr : arr) {
        // do thing with each int
        assert(itr == i);
        std::cout << itr << std::endl;
    }
    il2cpp_utils::NewSpecific<ListW<Il2CppObject*>>(1, 2, 3);
    std::cout << i << std::endl;

    arr.clear();
    arr.insert_at(0, 0);
    arr.emplace_back(0);
    arr.push_back(0);
    arr.erase(0);
    arr.find([](int&){return true;});
    arr.reverse_find([](int&) { return true; });
    arr.front();
    arr.back();
    arr.get(0);
    auto const arrConst = arr;
    arrConst.front();
    arrConst.back();
    arrConst.find([](int const&) { return true; });
    arrConst.reverse_find([](int const&) { return true; });
    arrConst.get(0);
    arrConst.to_array();
}

static void doThing2() {
    ListW<int> arr(nullptr);
    MethodInfo info;
    il2cpp_utils::RunMethodThrow(classof(Il2CppObject*), &info, arr);
    il2cpp_utils::RunMethodThrow<ListW<Il2CppObject*>>(classof(Il2CppObject*), &info);
}

#endif
