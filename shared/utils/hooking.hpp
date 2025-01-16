#pragma once

#include "../inline-hook/And64InlineHook.hpp"
#include "hook-tracker.hpp"
#include <type_traits>
#include <utility>
#include "utils.h"
#include "typedefs.h"
#include "logging.hpp"
#include "il2cpp-utils.hpp"
#include "convenience-hooks.hpp"

namespace Hooking {

/**
 * @brief The DeferredHooks class manages the registration and installation of hook functions.
 *
 * This class provides a mechanism to register hook installation functions that can be called later
 * to install hooks. It maintains a list of installation functions and provides methods to add to
 * this list and to call all registered functions at a later time.
 */
class DeferredHooks {
private:
    inline static std::vector<void (*)()>& getInstallFuncs() {
        static std::vector<void (*)()> installFuncs;
        return installFuncs;
    }

public:
    /// @brief Adds an installation function to the list of functions to be called during InstallHooks.
    /// @param installFunc The function to be added.
    inline static void AddInstallFunc(void (*installFunc)()) {
        getInstallFuncs().push_back(installFunc);
    }

    /// @brief Calls all installation functions added via AddInstallFunc and then clears the list.
    inline static void InstallHooks() {
        for (auto& func : getInstallFuncs()) {
            func();
        }

        // Why would we need to keep these around?
        getInstallFuncs().clear();
    }
};

/// @brief Macro to automatically register a deferred hook installation function.
/// @param name_ The name of the hook to be installed.
#define MAKE_DEFERRED_HOOK_INSTALL_WITH_AUTOLOGGER(name_) \
    __attribute((constructor)) void Hook_##name_##_Auto_Register() { \
        ::Hooking::DeferredHooks::AddInstallFunc([]() { \
            static constexpr auto logger = Paper::ConstLoggerContext(MOD_ID); \
            INSTALL_HOOK(logger, name_); \
        }); \
    }

/// @brief Macro to automatically register a deferred hook installation function for original hooks.
/// @param name_ The name of the hook to be installed.
#define MAKE_DEFERRED_ORIG_HOOK_INSTALL_WITH_AUTOLOGGER(name_) \
    __attribute((constructor)) void Hook_##name_##_Auto_Orig_Register() { \
        ::Hooking::DeferredHooks::AddInstallFunc([]() { \
            static constexpr auto logger = Paper::ConstLoggerContext(MOD_ID); \
            INSTALL_HOOK_ORIG(logger, name_); \
        }); \
    }

/// @brief Macro to automatically register a deferred direct hook installation function.
/// @param name_ The name of the hook to be installed.
/// @param addr_ The address that should be hooked.
#define MAKE_DEFERRED_DIRECT_HOOK_INSTALL_WITH_AUTOLOGGER(name_, addr_) \
    __attribute((constructor)) void Hook_##name_##_Auto_Register() { \
        ::Hooking::DeferredHooks::AddInstallFunc([]() { \
            static constexpr auto logger = Paper::ConstLoggerContext(MOD_ID); \
            INSTALL_HOOK_DIRECT(logger, name_, addr_); \
        }); \
    }

/// @brief Macro to automatically register a deferred hook installation function with specified logger.
/// @param logger_ The logger to be used during install.
/// @param name_ The name of the hook to be installed.
#define MAKE_DEFERRED_HOOK_INSTALL(logger_, name_) \
    __attribute((constructor)) void Hook_##name_##_Auto_Register() { \
        ::Hooking::DeferredHooks::AddInstallFunc([]() { \
            INSTALL_HOOK(logger_, name_); \
        }); \
    }

/// @brief Macro to automatically register a deferred hook installation function for original hooks with specified logger.
/// @param logger_ The logger to be used during install.
/// @param name_ The name of the hook to be installed.
#define MAKE_DEFERRED_ORIG_HOOK_INSTALL(logger_, name_) \
    __attribute((constructor)) void Hook_##name_##_Auto_Orig_Register() { \
        ::Hooking::DeferredHooks::AddInstallFunc([]() { \
            INSTALL_HOOK_ORIG(logger_, name_); \
        }); \
    }

/// @brief Macro to automatically register a deferred direct hook installation function with specified logger.
/// @param logger_ The logger to be used during install.
/// @param name_ The name of the hook to be installed.
/// @param addr_ The address that should be hooked.
#define MAKE_DEFERRED_DIRECT_HOOK_INSTALL(logger_, name_, addr_) \
    __attribute((constructor)) void Hook_##name_##_Auto_Register() { \
        ::Hooking::DeferredHooks::AddInstallFunc([]() { \
            INSTALL_HOOK_DIRECT(logger_, name_, addr_); \
        }); \
    }

/// @brief Macro to install all registered automatic hooks.
#define INSTALL_DEFERRED_HOOKS() ::Hooking::DeferredHooks::InstallHooks();

// For use in MAKE_HOOK_AUTO bodies.
// Currently unused.
template<std::size_t N, typename... TArgs>
requires (sizeof...(TArgs) == 0 && N >= 0)
decltype(auto) param_get([[maybe_unused]] TArgs&&... as) noexcept {
    static_assert(N < 0, "Parameter index out of bounds!");
}

template<std::size_t N, typename Tfirst, typename... Args, std::enable_if_t<N == 0, int>...>
decltype(auto) param_get(Tfirst&& first, [[maybe_unused]] Args&&... as) noexcept {
    return std::forward<Tfirst>(first);
}

template<std::size_t N, typename Tfirst, typename... Args, std::enable_if_t<N != 0, int>...>
decltype(auto) param_get([[maybe_unused]] Tfirst&& first, Args&&... as) noexcept {
    return param_get<N - 1>(std::forward<Args>(as)...);
}

// Gets the parameter at the given index (starting at 0, not including self) to return in a MAKE_HOOK_AUTO body.
#define PARAM(i) \
::Hooking::param_get<i>(__args...)

template<typename L>
concept is_logger = requires (L& l) {
    l.info("");
    l.debug("");
    l.error("");
    l.warn("");
    l.critical("");
};

template<typename T>
concept is_hook = requires {
    // Must have a name
    {T::name()} -> std::same_as<const char*>;
    // Must have a trampoline that returns the funcType
    {T::trampoline()} -> std::same_as<typename T::funcType*>;
    // Must have a hook that returns the funcType
    {T::hook()} -> std::same_as<typename T::funcType>;
};

template<typename T>
concept has_addr = requires {
    {T::addr()} -> std::same_as<void*>;
};

template<typename T>
concept is_addr_hook = is_hook<T> && has_addr<T>;

template<typename T>
concept is_findCall_hook = is_hook<T> && requires {
    // Must have a getInfo() method
    {T::getInfo()} -> std::same_as<const MethodInfo*>;
};

/// @brief Represents a type that provides the funcType from a provided method pointer type.
/// This type is used in determining hook checks.
template<typename T>
struct InternalMethodCheck;

template<typename R, typename... TArgs>
struct InternalMethodCheck<R (*)(TArgs...)> {
    using funcType = R (*)(TArgs...);
};
template<typename R, typename T, typename... TArgs>
struct InternalMethodCheck<R (T::*)(TArgs...)> {
    using funcType = R (*)(T*, TArgs...);
};

/// @brief Exposes the instance type of the provided instance method.
template<typename T>
struct InternalClassGetter {
    using instanceType = void;
};

template<typename R, typename T, typename... TArgs>
struct InternalClassGetter<R (T::*)(TArgs...)> {
    using instanceType = T;
};

template<auto Func, class T>
/// @brief Exposes a static wrapper method that forwards to the provided function pointer, wrapping it in IL2CPP_CATCH_HANDLER.
struct HookCatchWrapper;

template<auto Func, class R, class... TArgs>
struct HookCatchWrapper<Func, R (*)(TArgs...)> {
    static R wrapper(TArgs... args) {
        IL2CPP_CATCH_HANDLER(
            return Func(args...);
        )
    }
};

// TODO: Make a pending_install collection and add HookInfo to it to ensure installation
// Then, walk this at load time and install (could do so after il2cpp_functions::Init)

// Make an address-specified hook, that has a catch handler.
#define MAKE_HOOK(name_, addr_, retval, ...) \
struct Hook_##name_ { \
    constexpr static const char* name() { return #name_; } \
    constexpr static void* addr() { return (void*) addr_; } \
    using funcType = retval (*)(__VA_ARGS__); \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make an address-specified hook.
#define MAKE_HOOK_NO_CATCH(name_, addr_, retval, ...) \
struct Hook_##name_ { \
    constexpr static const char* name() { return #name_; } \
    constexpr static void* addr() { return (void*) addr_; } \
    using funcType = retval (*)(__VA_ARGS__); \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return hook_##name_; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that resolves the 'infoGet' expression an installs the hook to that MethodInfo*, that has a catch handler.
#define MAKE_HOOK_FIND_VERBOSE(name_, infoGet, retval, ...) \
struct Hook_##name_ { \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return infoGet; } \
    using funcType = retval (*)(__VA_ARGS__); \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that resolves the 'infoGet' expression an installs the hook to that MethodInfo*.
#define MAKE_HOOK_FIND_VERBOSE_NO_CATCH(name_, infoGet, retval, ...) \
struct Hook_##name_ { \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return infoGet; } \
    using funcType = retval (*)(__VA_ARGS__); \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return hook_##name_; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that finds a method that matches the signature provided and exists on the provided Il2CppClass* with the provided method name, that has a catch handler
#define MAKE_HOOK_FIND(name_, klass, mName, retval, ...) \
struct Hook_##name_ { \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find(klass, mName); } \
    using funcType = retval (*)(__VA_ARGS__); \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that finds a method that matches the signature provided and exists on the provided Il2CppClass* with the provided method name.
#define MAKE_HOOK_FIND_NO_CATCH(name_, klass, mName, retval, ...) \
struct Hook_##name_ { \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find(klass, mName); } \
    using funcType = retval (*)(__VA_ARGS__); \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return hook_##name_; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that finds a method that matches the signature provided and exists on the provided namespace and type name, with the provided method name, that has a catch handler.
#define MAKE_HOOK_FIND_CLASS(name_, namespaze, klassName, mName, retval, ...) \
struct Hook_##name_ { \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find(namespaze, klassName, mName); } \
    using funcType = retval (*)(__VA_ARGS__); \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that finds a method that matches the signature provided and exists on the provided namespace and type name, with the provided method name.
#define MAKE_HOOK_FIND_CLASS_NO_CATCH(name_, namespaze, klassName, mName, retval, ...) \
struct Hook_##name_ { \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find(namespaze, klassName, mName); } \
    using funcType = retval (*)(__VA_ARGS__); \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return hook_##name_; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that finds a method that matches the signature provided and exists on the provided Il2CppClass* with the provided method name.
// Ignores matching the first parameter, assuming it is an instance method.
// Also includes a catch handler.
#define MAKE_HOOK_FIND_INSTANCE(name_, klass, mName, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<typename ::il2cpp_utils::InstanceMethodConverter<funcType>::fType>::find(klass, mName); } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that finds a method that matches the signature provided and exists on the provided Il2CppClass* with the provided method name.
// Ignores matching the first parameter, assuming it is an instance method.
#define MAKE_HOOK_FIND_INSTANCE_NO_CATCH(name_, klass, mName, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<typename ::il2cpp_utils::InstanceMethodConverter<funcType>::fType>::find(klass, mName); } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return hook_##name_; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that finds a method that matches the signature provided and exists on the provided namespace and type name, with the provided method name.
// Ignores matching the first parameter, assuming it is an instance method.
// Also includes a catch handler.
#define MAKE_HOOK_FIND_CLASS_INSTANCE(name_, namespaze, klassName, mName, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<typename ::il2cpp_utils::InstanceMethodConverter<funcType>::fType>::find(namespaze, klassName, mName); } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that finds a method that matches the signature provided and exists on the provided namespace and type name, with the provided method name.
// Ignores matching the first parameter, assuming it is an instance method.
#define MAKE_HOOK_FIND_CLASS_INSTANCE_NO_CATCH(name_, namespaze, klassName, mName, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<typename ::il2cpp_utils::InstanceMethodConverter<funcType>::fType>::find(namespaze, klassName, mName); } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return hook_##name_; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that finds a method that matches the signature provided and exists on the provided namespace and type name, with the provided method name.
// THIS FUNCTION IS THE UNSAFE VARIANT, SUBTRACTS ONE FOR INSTANCE METHODS!
// Also includes a catch handler.
#define MAKE_HOOK_FIND_CLASS_UNSAFE_INSTANCE(name_, namespaze, klassName, mName, retval, ...) \
struct Hook_##name_ { \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find_unsafe(namespaze, klassName, mName, true); } \
    using funcType = retval (*)(__VA_ARGS__); \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that finds a method that matches the signature provided and exists on the provided namespace and type name, with the provided method name.
// THIS FUNCTION IS THE UNSAFE VARIANT, SUBTRACTS ONE FOR INSTANCE METHODS!
#define MAKE_HOOK_FIND_CLASS_UNSAFE_INSTANCE_NO_CATCH(name_, namespaze, klassName, mName, retval, ...) \
struct Hook_##name_ { \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find_unsafe(namespaze, klassName, mName, true); } \
    using funcType = retval (*)(__VA_ARGS__); \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return hook_##name_; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that finds a method that matches the signature provided and exists on the provided namespace and type name, with the provided method name.
// THIS FUNCTION IS THE UNSAFE VARIANT!
// Also includes a catch handler.
#define MAKE_HOOK_FIND_CLASS_UNSAFE_STATIC(name_, namespaze, klassName, mName, retval, ...) \
struct Hook_##name_ { \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find_unsafe(namespaze, klassName, mName); } \
    using funcType = retval (*)(__VA_ARGS__); \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that finds a method that matches the signature provided and exists on the provided namespace and type name, with the provided method name.
// THIS FUNCTION IS THE UNSAFE VARIANT!
#define MAKE_HOOK_FIND_CLASS_UNSAFE_STATIC_NO_CATCH(name_, namespaze, klassName, mName, retval, ...) \
struct Hook_##name_ { \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find_unsafe(namespaze, klassName, mName); } \
    using funcType = retval (*)(__VA_ARGS__); \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return hook_##name_; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that would be installed in a particular address, but ensures the signature matches the provided method pointer.
// Also includes a catch handler.
#define MAKE_HOOK_CHECKED_ADDR(name_, mPtr, addr_, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); \
    constexpr static const char* name() { return #name_; } \
    constexpr static void* addr() { return addr_; } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that would be installed in a particular address, but ensures the signature matches the provided method pointer.
#define MAKE_HOOK_CHECKED_ADDR_NO_CATCH(name_, mPtr, addr_, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); \
    constexpr static const char* name() { return #name_; } \
    constexpr static void* addr() { return addr_; } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return hook_##name_; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that ensures the signature matches the provided method pointer and finds a matching method from a class and method name.
// Also includes a catch handler.
#define MAKE_HOOK_CHECKED_FIND(name_, mPtr, klass, mName, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find(klass, mName); } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that ensures the signature matches the provided method pointer and finds a matching method from a class and method name.
#define MAKE_HOOK_CHECKED_FIND_NO_CATCH(name_, mPtr, klass, mName, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find(klass, mName); } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return hook_##name_; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that ensures the signature matches the provided method pointer and finds a matching method from namespace, name, and method name.
// Also includes a catch handler.
#define MAKE_HOOK_CHECKED_FIND_CLASS(name_, mPtr, namespaze, klassName, mName, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find(namespaze, klassName, mName); } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that ensures the signature matches the provided method pointer and finds a matching method from namespace, name, and method name.
#define MAKE_HOOK_CHECKED_FIND_CLASS_NO_CATCH(name_, mPtr, namespaze, klassName, mName, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find(namespaze, klassName, mName); } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return hook_##name_; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that ensures the signature matches the provided method pointer and finds a matching method from a class and method name.
// Also includes a catch handler.
#define MAKE_HOOK_CHECKED_INSTANCE_FIND(name_, mPtr, mName, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    using classType = ::Hooking::InternalClassGetter<decltype(mPtr)>::instanceType; \
    static_assert(!std::is_same_v<classType, void>, "MAKE_HOOK_INSTANCE_FIND was not provided an instance method pointer!"); \
    static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find(classof(classType), mName); } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that ensures the signature matches the provided method pointer and finds a matching method from a class and method name.
#define MAKE_HOOK_CHECKED_INSTANCE_FIND_NO_CATCH(name_, mPtr, mName, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    using classType = ::Hooking::InternalClassGetter<decltype(mPtr)>::instanceType; \
    static_assert(!std::is_same_v<classType, void>, "MAKE_HOOK_INSTANCE_FIND was not provided an instance method pointer!"); \
    static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::MethodTypeCheck<funcType>::find(classof(classType), mName); } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return hook_##name_; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

#ifndef BS_HOOK_MATCH_UNSAFE
#define MATCH_HOOKABLE_ASSERT(mPtr) ::il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::size >= 0x5 * sizeof(uint32_t) && ::il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::addrs != 0x0
#else
#define MATCH_HOOKABLE_ASSERT(mPtr) true
#endif

// Make a hook that uses the provided method pointer in a match an ensures the signature matches.
// This should be your go-to hook macro when hooking anything that has a codegen equivalent.
// Also includes a catch handler.
#define MAKE_HOOK_MATCH(name_, mPtr, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    static_assert(MATCH_HOOKABLE_ASSERT(mPtr)); \
    static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::methodInfo(); } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that uses the provided method pointer in a match an ensures the signature matches.
// This should be your go-to hook macro when hooking anything that has a codegen equivalent.
#define MAKE_HOOK_MATCH_NO_CATCH(name_, mPtr, retval, ...) \
struct Hook_##name_ { \
    using funcType = retval (*)(__VA_ARGS__); \
    static_assert(MATCH_HOOKABLE_ASSERT(mPtr)); \
    static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::methodInfo(); } \
    static funcType* trampoline() { return &name_; } \
    static inline retval (*name_)(__VA_ARGS__) = nullptr; \
    static funcType hook() { return hook_##name_; } \
    static retval hook_##name_(__VA_ARGS__); \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// TODO: Remove all of these macros and replace it with just one or MAYBE two-- if people want to do it themselves
// they can implement the structure themselves

template<class T>
struct TypeConv {
    using type = T;
    static T make(T p) {
        return p;
    }
    static T convert(T p) {
        return p;
    }
};

template<>
struct TypeConv<void> {
    using type = void;
};

template<class T>
requires (il2cpp_utils::has_il2cpp_conversion<T>)
struct TypeConv<T> {
    using type = void*;
    static T make(void* p) {
        return T(p);
    }
    static void* convert(T v) {
        return v.convert();
    }
};

template<auto F>
struct HookWrapperCompose;

template<class Ret, class... TArgs, Ret (*func)(TArgs...)>
struct HookWrapperCompose<func> {
    static typename TypeConv<Ret>::type wrapper(typename TypeConv<TArgs>::type... args) {
        if constexpr (std::is_same_v<typename TypeConv<Ret>::type, void>) {
            func(TypeConv<TArgs>::make(args)...);
        } else {
            return TypeConv<Ret>::convert(func(TypeConv<TArgs>::make(args)...));
        }
    }
};

template<class F>
struct HookWrapperInvoke;

template<class R, class... TArgs>
struct HookWrapperInvoke<R (*)(TArgs...)> {
    static R wrapper(typename TypeConv<R>::type (*func)(typename TypeConv<TArgs>::type...), TArgs... args) {
        if constexpr (std::is_same_v<R, void>) {
            func(TypeConv<TArgs>::convert(args)...);
        } else {
            return TypeConv<R>::make(func(TypeConv<TArgs>::convert(args)...));
        }
    }
};

#define MAKE_HOOK_WRAPPER(name_, mPtr, retval, ...) \
struct Hook_##name_ { \
    static retval hook_##name_(__VA_ARGS__); \
    using funcType = decltype(&::Hooking::HookWrapperCompose<&hook_##name_>::wrapper); \
    /* static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); */ \
    constexpr static const char* name() { return #name_; } \
    static const MethodInfo* getInfo() { return ::il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::methodInfo(); } \
    static funcType* trampoline() { return &orig_base; } \
    static inline funcType orig_base = nullptr; \
    template<class... TArgs> \
    static inline retval name_(TArgs... args) { \
        if constexpr (std::is_same_v<retval, void>) { \
            ::Hooking::HookWrapperInvoke<decltype(&hook_##name_)>::wrapper(orig_base, args...); \
        } else { \
            return ::Hooking::HookWrapperInvoke<decltype(&hook_##name_)>::wrapper(orig_base, args...); \
        } \
    } \
    static funcType hook() { return &::Hooking::HookWrapperCompose<::Hooking::HookCatchWrapper<&hook_##name_, decltype(&hook_##name_)>::wrapper>::wrapper; } \
}; \
retval Hook_##name_::hook_##name_(__VA_ARGS__)

// Make a hook that writes the signature based off of the provided static method.
// To access parameters, see the PARAM(i) macro.
// #define MAKE_HOOK_STATIC_AUTO(name, ...) \
// template<class T> \
// struct __##name; \
// template<typename R, typename... TArgs> \
// struct __##name<R (*)(TArgs...)> { \
//     static inline R (*name)(TArgs... __args); \
//     static R hook_##name(TArgs... __args) { \
//         __VA_ARGS__ \
//     } \
// }; \
// template<typename R, typename T, typename... TArgs> \
// struct __##name<R (T::*)(TArgs...)> { \
//     static_assert(!std::is_same_v<R, R>, "Attempting to MAKE_HOOK_STATIC_AUTO with an instance method! See MAKE_HOOK_INSTANCE_AUTO instead!"); \
// };

// // Make a hook that writes the signature based off of the provided static method.
// // To access parameters, use `self` and see the PARAM(i) macro (where self is not included as a PARAM)
// // Should only be used with INSTALL_HOOK_AUTO
// #define MAKE_HOOK_INSTANCE_AUTO(name, ...) \
// template<class T> \
// struct __##name; \
// template<typename R, typename T, typename... TArgs> \
// struct __##name<R (T::*)(TArgs...)> { \
//     static inline R (*name)(T* self, TArgs... __args); \
//     static R hook_##name(T* self, TArgs... __args) { \
//         __VA_ARGS__ \
//     } \
// }; \
// template<typename R, typename... TArgs> \
// struct __##name<R (*)(TArgs...)> { \
//     static_assert(!std::is_same_v<R, R>, "Attempting to MAKE_HOOK_INSTANCE_AUTO with a static method! See MAKE_HOOK_STATIC_AUTO instead!"); \
// };

template<typename T, typename L, bool track = true>
inline void __InstallHook(L& logger, void* addr) {
    #ifndef SUPPRESS_MACRO_LOGS
    logger.info("Installing hook: {} to offset: {}", T::name(), fmt::ptr(addr));
    #endif
    #ifdef __aarch64__
    if constexpr (track) {
        HookInfo info(T::name(), addr, (void*) T::hook());
        A64HookFunction(addr, (void*) T::hook(), (void**) T::trampoline());
        info.orig = (void*) *T::trampoline();
        HookTracker::AddHook(info);
    } else {
        A64HookFunction(addr, (void*) T::hook(), (void**) T::trampoline());
    }
    #else
    registerInlineHook((uint32_t) addr, (uint32_t) T::hook(), (uint32_t **) T::trampoline());
    inlineHook((uint32_t) addr);
    #endif
}

template<typename T, typename L>
requires (is_addr_hook<T> && !is_findCall_hook<T> && is_logger<L>)
void InstallHook(L& logger) {
    // Install T assuming it is an address hook.
    auto addr = (void*) getRealOffset(T::addr());
    __InstallHook<T>(logger, addr);
}
template<typename T, typename L>
requires (is_findCall_hook<T> && !is_addr_hook<T> && is_logger<L>)
void InstallHook(L& logger) {
    // Install T assuming it is a hook that should call FindMethod.
    auto info = T::getInfo();
    if (!info) {
        #ifndef SUPPRESS_MACRO_LOGS
        logger.critical("Attempting to install hook: {}, but method could not be found!", T::name());
        #endif
        SAFE_ABORT();
    }
    auto addr = (void*) info->methodPointer;
    __InstallHook<T>(logger, addr);
}
template<typename T, typename L>
requires (is_findCall_hook<T> && !is_addr_hook<T> && is_logger<L>)
void InstallOrigHook(L& logger) {
    // Install T assuming it is a hook that should call FindMethod.
    auto info = T::getInfo();
    if (!info) {
        #ifndef SUPPRESS_MACRO_LOGS
        logger.critical("Attempting to install hook: {}, but method could not be found!", T::name());
        #endif
        SAFE_ABORT();
    }
    auto addr = (void*) info->methodPointer;
    auto* origAddr = const_cast<void*>(HookTracker::GetOrig(addr));
    if (origAddr != addr) { \
        auto* hooks = const_cast<std::unordered_map<const void*, std::list<HookInfo>>*>(HookTracker::GetHooks());
        auto itr = hooks->find(addr);
        if (itr != hooks->end() && itr->second.size() > 0) {
            itr->second.front().orig = (void*) *T::trampoline();
        }
    }
    __InstallHook<T, L, false>(logger, origAddr);
}
template<typename T, typename L>
requires (is_hook<T> && is_logger<L>)
void InstallHookDirect(L& logger, void* dst) {
    // Install T into the specified address. Null checks dst.
    if (!dst) {
        #ifndef SUPPRESS_MACRO_LOGS
        logger.critical("Attempting to install direct hook: {}, but was installing to an invalid destination!", T::name());
        #endif
        SAFE_ABORT();
    }
    __InstallHook<T>(logger, dst);
}

// Installs the provided hook using the logger provided.
// This properly specializes based off of whichever MAKE_HOOK macro you used, but is only valid if the name is from a MAKE_HOOK... macro.
#define INSTALL_HOOK(logger, name) ::Hooking::InstallHook<Hook_##name>(logger);

// Installs the provided hook using the logger provided to the address specified directly.
// This is only valid if the name is from a MAKE_HOOK... macro.
#define INSTALL_HOOK_DIRECT(logger, name, addr) ::Hooking::InstallHookDirect<Hook_##name>(logger, addr);

// Installs the provided hook using the logger provided to the 'orig' point of the hook.
// This is only valid if the name is from a MAKE_HOOK macro that does not use a fixed offset.
// This also ensures HookTracker validity after the hooking process.
#define INSTALL_HOOK_ORIG(logger, name) ::Hooking::InstallOrigHook<Hook_##name>(logger);

#define INSTALL_HOOK_ON_DLOPEN_WITH_AUTOLOGGER(name_) \
    __attribute((constructor)) void Hook_##name_##_Dlopen_Install() { \
        static constexpr auto logger = Paper::ConstLoggerContext(MOD_ID); \
        INSTALL_HOOK(logger, name_); \
    }

#define INSTALL_DIRECT_HOOK_ON_DLOPEN_WITH_AUTOLOGGER(name_, addr_) \
    __attribute((constructor)) void Hook_##name_##_Dlopen_Direct_Install() { \
        static constexpr auto logger = Paper::ConstLoggerContext(MOD_ID); \
        INSTALL_HOOK_DIRECT(logger, name_, addr_); \
    }

#define INSTALL_ORIG_HOOK_ON_DLOPEN_WITH_AUTOLOGGER(name_) \
    __attribute((constructor)) void Hook_##name_##_Dlopen_Orig_Install() { \
        static constexpr auto logger = Paper::ConstLoggerContext(MOD_ID); \
        INSTALL_HOOK_ORIG(logger, name_); \
    }

#define INSTALL_HOOK_ON_DLOPEN(logger, name_) \
    __attribute((constructor)) void Hook_##name_##_Dlopen_Install() { \
        INSTALL_HOOK(logger, name_); \
    }

#define INSTALL_DIRECT_HOOK_ON_DLOPEN(logger, name_, addr_) \
    __attribute((constructor)) void Hook_##name_##_Dlopen_Direct_Install() { \
        INSTALL_HOOK_DIRECT(logger, name_, addr_); \
    }

#define INSTALL_ORIG_HOOK_ON_DLOPEN(logger, name_) \
    __attribute((constructor)) void Hook_##name_##_Dlopen_Orig_Install() { \
        INSTALL_HOOK_ORIG(logger, name_); \
    }

// TODO: Not yet implemented
#define UNINSTALL_HOOK(logger, name) void

// TODO: Not yet implemented
#define UNINSTALL_HOOK_DIRECT(logger, name, addr) void
}
