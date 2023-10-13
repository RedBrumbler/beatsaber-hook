#ifdef TEST_ARRAY

#include <type_traits>
#include "utils/type-concepts.hpp"
#include "utils/value-wrapper-type.hpp"
#include "shared/utils/typedefs-array.hpp"
#include "utils/base-wrapper-type.hpp"
#include <iostream>
#include <assert.h>

class SomeWrapper : public ::bs_hook::Il2CppWrapperType {
    public:
        static constexpr auto __IL2CPP_VALUE_TYPE = false;
        SomeWrapper(void* i) : ::bs_hook::Il2CppWrapperType(i) {}

        virtual ~SomeWrapper() {}

        void method() {}
};

template<>
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<SomeWrapper> {
    static inline Il2CppClass* get() {
        return classof(Il2CppObject*);
    }
};

static_assert(::il2cpp_utils::il2cpp_reference_type_requirements<SomeWrapper>);
static_assert(::il2cpp_utils::il2cpp_reference_type<SomeWrapper>);
static_assert(::il2cpp_utils::RefTypeTrait<SomeWrapper>::value);
static_assert(sizeof(SomeWrapper) == 0x10);
static_assert(std::is_same_v<decltype(std::remove_pointer_t<ArrayW<SomeWrapper>::Ptr>::values), void*[0]>);

class MatchingWrapper : public ::bs_hook::Il2CppWrapperType {
    public:
        static constexpr auto __IL2CPP_VALUE_TYPE = false;
        MatchingWrapper(void* i) : ::bs_hook::Il2CppWrapperType(i) {}

};

template<>
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<MatchingWrapper> {
    static inline Il2CppClass* get() {
        return classof(Il2CppObject*);
    }
};

static_assert(::il2cpp_utils::il2cpp_reference_type_requirements<MatchingWrapper>);
static_assert(::il2cpp_utils::il2cpp_reference_type<MatchingWrapper>);
static_assert(::il2cpp_utils::RefTypeTrait<MatchingWrapper>::value);
static_assert(sizeof(MatchingWrapper) == 0x8);
static_assert(std::is_same_v<decltype(std::remove_pointer_t<ArrayW<MatchingWrapper>::Ptr>::values), MatchingWrapper[0]>);

struct Color : public ::bs_hook::ValueTypeWrapper<0x10> {
    static constexpr auto __IL2CPP_VALUE_TYPE = true;
    Color(std::array<std::byte, 0x10> i) noexcept : ::bs_hook::ValueTypeWrapper<0x10>(i) {}
};

template<>
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<Color> {
    static inline Il2CppClass* get() {
        return il2cpp_utils::GetClassFromName("UnityEngine", "Color");
    }
};

static_assert(::il2cpp_utils::il2cpp_value_type_requirements<Color>);
static_assert(::il2cpp_utils::il2cpp_value_type<Color>);
static_assert(::il2cpp_utils::ValueTypeTrait<Color>::value);
static_assert(sizeof(Color) == 0x10);
static_assert(std::is_same_v<decltype(std::remove_pointer_t<ArrayW<Color>::Ptr>::values), Color[0]>);

static_assert(std::is_same_v<decltype(std::remove_pointer_t<ArrayW<int>::Ptr>::values), int[0]>);
static_assert(std::is_same_v<decltype(std::remove_pointer_t<ArrayW<int>::Ptr>::values), int[0]>);

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
    std::cout << arr.Length() << std::endl;
    std::cout << static_cast<Array<int*>*>(arr3) << std::endl;
    // Should be allowed to cast back
    std::cout << static_cast<Array<int>*>(arr) << std::endl;
    std::cout << i << std::endl;
    // Should be simply nullptr
    std::cout << static_cast<Array<float>*>(initThing) << std::endl;

    /// get first element that fulfills the predicate
    arr.FirstOrDefault();
    arr3.First();
    arr.FirstOrDefault([](auto x){ return x == 0; });
    arr3.First([](auto x){ return x == 0; });

    /// get first reverse iter element that fulfills the predicate
    arr.FirstOrDefault();
    arr3.First();
    arr.LastOrDefault([](auto x){ return x == 0; });
    arr3.Last([](auto x){ return x == 0; });

    auto wrapperArr = ArrayW<SomeWrapper>(10);
    wrapperArr[0] = nullptr;
    // use operator-> to directly access the underlying wrapper type
    wrapperArr[0]->method();

    // apparently an iter with operator* gets dereffed automatically
    // also auto& or const auto& is just not possible with wrapper types now, unless we were to specify their sizes to match
    for (auto v : wrapperArr) {
        static_assert(std::is_same_v<decltype(v), SomeWrapper>);

        v = nullptr;
        v.method();
    }

    // when using operator[] you instead get a IndexRef<T>
    for(il2cpp_array_size_t i = 0; i < wrapperArr.size(); i++) {
        auto v = wrapperArr[i];
        static_assert(std::is_same_v<decltype(v), IndexRef<SomeWrapper>>);

        v = nullptr;
    }
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
