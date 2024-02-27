#include <iostream>
#ifdef TEST_BETTER_SPAN

#include <initializer_list>
#include "shared/utils/better_span.hpp"

static void run() {
    std::vector<int> vec;
    std::initializer_list<int> initList = { 5, 2, 3, 1 };
    std::array<int, 4> arr{ 1, 1, 2, 3 };
    std::span<int, 4> span = arr;

    bs_hook::better_span<int> vecSpan = vec;
    // initializer list only supports const
    bs_hook::better_span<int const> initSpan = initList;
    bs_hook::better_span<int const> initSpan2 = {1,1,1,1};
    bs_hook::better_span<int> arrSpan = arr;
    bs_hook::better_span<int> spanSpan = span;

    std::span<int const> spanSpanSpan = spanSpan;

    // remove unused variable warnings
    std::cout << vecSpan.data() << initSpan.data() << initSpan2.data() << arrSpan.data() << spanSpan.data() << spanSpanSpan.data() << std::endl;
}
static void runConst() {
    std::vector<int> vec;
    std::initializer_list<int> initList = { 5, 2, 3, 1 };
    std::array<int, 4> arr{ 1, 1, 2, 3 };
    std::span<int const, 4> span = arr;

    bs_hook::better_span<int const> vecSpan = vec;
    // initializer list only supports const
    bs_hook::better_span<int const> initSpan = initList;
    bs_hook::better_span<int const> arrSpan = arr;
    bs_hook::better_span<int const, 4> spanSpan = span;


    std::span<int const, 4> spanSpanSpan = spanSpan;
    
    // remove unused variable warnings
    std::cout << vecSpan.data() << initSpan.data() << arrSpan.data() << spanSpan.data() << spanSpanSpan.data() << std::endl;
}

#include "shared/utils/typedefs-array.hpp"
#include "shared/utils/typedefs-list.hpp"

static void run2() {
    ArrayW<int> arr;
    ListW<int> list;

    bs_hook::better_span<int> arrSpan = arr;
    bs_hook::better_span<int> listSpan = list;

    // remove unused variable warnings
    std::cout << arrSpan.data() << listSpan.data() << std::endl;
}
static void runConst2() {
    ArrayW<int> arr;
    ListW<int> list;

    bs_hook::better_span<int const> arrSpan = arr;
    bs_hook::better_span<int const> listSpan = list;

    // remove unused variable warnings
    std::cout << arrSpan.data() << listSpan.data() << std::endl;
}

#endif