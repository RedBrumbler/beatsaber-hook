#ifdef TEST_THREAD
#include "utils/il2cpp-utils.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-parameter"

static void thread_method() {}
static void thread_method_value(int v) {}
static void thread_method_lvalue(int& v) {}
static void thread_method_rvalue(int&& v) {}

struct ThreadTest {
    void method() {}

    void method_value(int v) {}
    void method_lvalue(int& v) {}
    void method_rvalue(int&& v) {}

    // can we do all the same things from an instance of a struct
    void test_thread();

    void some_call() {};
};

// test both il2cpp aware thread and std::thread for equivalence
#define IL2CPP_THREAD_TEST(...) \
    std::thread(__VA_ARGS__).join(); \
    il2cpp_utils::il2cpp_aware_thread(__VA_ARGS__).join()

void test_thread() {
    // can we make a 0 arg lambda thread?
    IL2CPP_THREAD_TEST([](){});

    IL2CPP_THREAD_TEST(&thread_method);

    int v = 0;

    // can we capture?
    IL2CPP_THREAD_TEST([=](){ int b = v + 1; });
    IL2CPP_THREAD_TEST([&](){ v += 1;});
    IL2CPP_THREAD_TEST([v](){ int b = v + 1; });
    IL2CPP_THREAD_TEST([value = v](){ int b = value + 1; });
    IL2CPP_THREAD_TEST([value = &v](){ *value += 1; });
    IL2CPP_THREAD_TEST([&v](){ v += 1; });

    // can we make a lambda that accepts an integer from lvalue & rvalue?
    IL2CPP_THREAD_TEST([](int v){}, v);
    IL2CPP_THREAD_TEST([](int v){}, 10);

    // pass rvalue into lvalue
    // IL2CPP_THREAD_TEST([](int& v){}, 10); // should not work
    // pass rvalue into rvalue
    IL2CPP_THREAD_TEST([](int&& v){}, 10); // should work
    // pass lvalue into lvalue
    // IL2CPP_THREAD_TEST([]( int& v){}, v); // should work
    // pass lvalue into rvalue
    IL2CPP_THREAD_TEST([](int&& v){}, v); // should work

    // the same for a method?
    IL2CPP_THREAD_TEST(&thread_method_value, v); // should work
    IL2CPP_THREAD_TEST(&thread_method_value, 10); // should work

    // can we pass the value as appropriate?
    // lvalue into lvalue
    // IL2CPP_THREAD_TEST(&thread_method_lvalue, v); // should not work
    // lvalue into rvalue
    IL2CPP_THREAD_TEST(&thread_method_rvalue, v); // should work
    // rvalue into lvalue
    // IL2CPP_THREAD_TEST(&thread_method_lvalue, 10); // should not work
    // rvalue into rvalue
    IL2CPP_THREAD_TEST(&thread_method_rvalue, 10); // should work

    ThreadTest tt;
    tt.test_thread();

    il2cpp_utils::il2cpp_aware_thread([]{
        // getting current jni env since we are attached
        auto env = il2cpp_utils::threading::get_current_env();

        // getting current thread id as a test
        auto id = il2cpp_utils::threading::current_thread_id();
    }).join();
}

void ThreadTest::test_thread() {
    // can we make a 0 arg lambda thread?
    IL2CPP_THREAD_TEST([](){});
    IL2CPP_THREAD_TEST(&ThreadTest::method, this);

    int v = 0;

    // can we capture?
    IL2CPP_THREAD_TEST([this](){ some_call(); });
    IL2CPP_THREAD_TEST([=](){ int b = v + 1; });
    IL2CPP_THREAD_TEST([&](){ some_call(); });
    IL2CPP_THREAD_TEST([v](){ int b = v + 1; });
    IL2CPP_THREAD_TEST([value = v](){ int b = value + 1; });
    IL2CPP_THREAD_TEST([value = &v](){ *value += 1; });
    IL2CPP_THREAD_TEST([&v](){ v += 1; });

    // can we make a lambda that accepts an integer from lvalue & rvalue?
    IL2CPP_THREAD_TEST([](ThreadTest* self, int v){}, this, v);
    IL2CPP_THREAD_TEST([](ThreadTest* self, int v){}, this, 10);

    // the same for a method?
    IL2CPP_THREAD_TEST(&ThreadTest::method_value, this, v);
    IL2CPP_THREAD_TEST(&ThreadTest::method_value, this, 10);

    // can we pass the value as appropriate?
    // IL2CPP_THREAD_TEST(&ThreadTest::method_lvalue, this, v); // should not work
    IL2CPP_THREAD_TEST(&ThreadTest::method_rvalue, this, 10);
}


// #define IL2CPP_ASYNC_TEST(...) \
//     il2cpp_utils::il2cpp_async(__VA_ARGS__).wait(); \
//     std::async(__VA_ARGS__).wait()

#define IL2CPP_ASYNC_TEST(...) \
    il2cpp_utils::il2cpp_async(__VA_ARGS__).wait(); \
    std::async(__VA_ARGS__).wait()

static void func() {}
static bool func2(int v) { return v; }
static int* func3(int& v) { return &v; }
static float func4(int&& v) { return v; }

void test_async() {
    IL2CPP_ASYNC_TEST([]{ return 1; });
    IL2CPP_ASYNC_TEST([]{ return 1; });
    float v = 0;
    // not allowed on std::async
    // IL2CPP_ASYNC_TEST([](float& v){ return v + 1; }, v);
    IL2CPP_ASYNC_TEST(&func);
    IL2CPP_ASYNC_TEST(&func2, 2);
    int a = 2;
    // not allowed on std::async
    // IL2CPP_ASYNC_TEST(&func3, a);
    IL2CPP_ASYNC_TEST(&func4, std::move(a));
    IL2CPP_ASYNC_TEST(&func4, 1);
    IL2CPP_ASYNC_TEST([&v](int b){ return v = b; }, 1);
}
#pragma clang diagnostic pop

#endif
