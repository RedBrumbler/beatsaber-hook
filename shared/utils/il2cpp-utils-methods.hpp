#ifndef IL2CPP_UTILS_METHODS
#define IL2CPP_UTILS_METHODS

#include <type_traits>
#pragma pack(push)

#include "il2cpp-functions.hpp"
#include "logging.hpp"
#include <vector>
#include "il2cpp-utils-exceptions.hpp"
#include "il2cpp-utils-classes.hpp"
#include "il2cpp-type-check.hpp"
#include "utils.h"
#include "il2cpp-tabledefs.h"
#include <array>
#include <exception>

#if __has_include(<concepts>)
#include <concepts>
#ifndef BS_HOOK_NO_CONCEPTS
#define BS_HOOK_USE_CONCEPTS
#endif
#endif

// ALWAYS define this here. It will NOT be redefined in typedefs.h anymore.
typedef struct Il2CppExceptionWrapper Il2CppExceptionWrapper;
typedef struct Il2CppExceptionWrapper
{
#if RUNTIME_MONO
    MonoException* ex;
#ifdef __cplusplus
    Il2CppExceptionWrapper(MonoException* ex) : ex(ex) {}
#endif //__cplusplus
#else
    Il2CppException* ex;
#ifdef __cplusplus
    Il2CppExceptionWrapper(Il2CppException* ex) : ex(ex) {}
#endif //__cplusplus
#endif
} Il2CppExceptionWrapper;

namespace il2cpp_utils {

    /// @brief How to create an il2cpp object.
    enum struct CreationType {
        /// @brief Created object is a C# object, it may be GC'd.
        Temporary,
        /// @brief Created object is manual, it must be freed explicitly (via delete).
        Manual
    };

    /// @brief Manually creates an instance of the provided Il2CppClass*.
    /// The created instance's type initializer will NOT execute on another thread! Be warned!
    /// Must be freed using gc_free_specific!
    /// @param klass The Il2CppClass* to create an instance of.
    /// @return The created instance, or nullptr if it failed for any reason.
    Il2CppObject* createManual(const Il2CppClass* klass) noexcept;
    /// @brief Manually creates an instance of the provided Il2CppClass*.
    /// The created instance's type initializer will NOT execute on another thread! Be warned!
    /// Must be freed using gc_free_specific!
    /// This function will throw a exceptions::StackTraceException on failure.
    /// @param klass The Il2CppClass* to create an instance of.
    /// @return The created instance.
    Il2CppObject* createManualThrow(Il2CppClass* const klass);

    struct FindMethodInfo {
        Il2CppClass* klass = nullptr;
        ::std::string name;
        Il2CppClass* returnType = nullptr;
        ::std::vector<const Il2CppClass*> genTypes;
        ::std::vector<const Il2CppType*> argTypes;

        #ifndef BS_HOOK_USE_CONCEPTS
        template <typename T, typename... TParams,
            ::std::enable_if_t<!::std::is_convertible_v<T, ::std::string_view>, int> = 0>
        #else
        template<typename T, typename... TParams>
        requires (!::std::is_convertible_v<T, std::string_view>)
        #endif
        FindMethodInfo(T&& classOrInstance, ::std::string_view methodName, TParams&&... paramTypes) {
            klass = ExtractClass(classOrInstance);
            name = methodName;

            if constexpr (sizeof...(TParams) > 0) {
                if constexpr (sizeof...(TParams) == 1 && (... && is_vector<::std::decay_t<TParams>>::value))
                    argTypes = TypesFrom(paramTypes...);
                else
                    argTypes = TypesFrom({paramTypes...});
            }
        }

        template<typename T, size_t sz>
        FindMethodInfo(T&& classOrInstance, ::std::string_view methodName, std::array<const Il2CppType*, sz>& params) {
            klass = ExtractClass(classOrInstance);
            name = methodName;

            argTypes = std::vector<const Il2CppType*>(params.begin(), params.end());
        }

        #ifndef BS_HOOK_USE_CONCEPTS
        template <typename T, typename G, typename... TArgs,
            ::std::enable_if_t<!::std::is_convertible_v<Il2CppType*, G>, int> = 0>
        #else
        template<typename T, typename G, typename... TArgs>
        requires (!::std::is_convertible_v<G, Il2CppType*> && !::std::is_convertible_v<G, const Il2CppType*>)
        #endif
        FindMethodInfo(T&& classOrInstance, ::std::string_view methodName, ::std::span<G> genericArgs, TArgs&&... args)
            : FindMethodInfo(classOrInstance, methodName, args...)
        {
            genTypes = ClassesFrom(genericArgs);
        }

        #ifndef BS_HOOK_USE_CONCEPTS
        template <typename T, typename R, typename... TArgs,
            ::std::enable_if_t<!::std::is_convertible_v<R, ::std::string_view>, int> = 0>
        #else
        template<typename T, typename R, typename... TArgs>
        requires (!::std::is_convertible_v<R, std::string_view>)
        #endif
        FindMethodInfo(T&& classOrInstance, R returnTypeOrClass, ::std::string_view methodName, TArgs&&... args)
            : FindMethodInfo(classOrInstance, methodName, args...)
        {
            returnType = ExtractClass(returnTypeOrClass);
        }

        template <typename... TArgs>
        FindMethodInfo(::std::string_view namespaceName, ::std::string_view className, TArgs&&... args) : FindMethodInfo(GetClassFromName(namespaceName, className), args...) {}

        bool operator==(FindMethodInfo const&) const = default;
        bool operator!=(FindMethodInfo const&) const = default;
    };

    const MethodInfo* ResolveMethodWithSlot(Il2CppClass* klass, uint16_t slot) noexcept;

    const MethodInfo* ResolveVtableSlot(Il2CppClass* klass, Il2CppClass* declaringClass, uint16_t slot) noexcept;

    const MethodInfo* ResolveVtableSlot(Il2CppClass* klass, ::std::string_view declaringNamespace, ::std::string_view declaringClassName, uint16_t slot) noexcept;

    #ifndef BS_HOOK_USE_CONCEPTS
    template<typename T, typename... TArgs,
        ::std::enable_if_t<!::std::is_same_v<T, Il2CppClass*>, int> = 0>
    #else
    template<typename T, typename... TArgs>
    requires (!::std::is_same_v<T, Il2CppClass*>)
    #endif
    const MethodInfo* ResolveVtableSlot(T&& instance, TArgs&&... args) noexcept {
        return ResolveVtableSlot(::il2cpp_utils::ExtractClass(instance), args...);
    }

    template<class T>
    Il2CppObject* ToIl2CppObject(T&& arg) {
        il2cpp_functions::Init();

        using Dt = ::std::decay_t<T>;
        if constexpr (::std::is_same_v<Dt, Il2CppType*> || ::std::is_same_v<Dt, Il2CppClass*>) {
            return nullptr;
        }
        static auto& logger = getLogger();
        auto* klass = RET_0_UNLESS(logger, ::il2cpp_utils::ExtractClass(arg));
        return il2cpp_functions::value_box(klass, &arg);
    }

    template<class T>
    void* ExtractValue(T&& arg) {
        il2cpp_functions::Init();

        using Dt = ::std::decay_t<T>;
        if constexpr (::std::is_same_v<Dt, Il2CppType*> || ::std::is_same_v<Dt, Il2CppClass*>) {
            return nullptr;
        } else if constexpr (::std::is_pointer_v<Dt>) {
            if constexpr (::std::is_base_of_v<Il2CppObject, ::std::remove_pointer_t<Dt>>) {
                if (arg) {
                    auto* klass = il2cpp_functions::object_get_class(reinterpret_cast<Il2CppObject*>(arg));
                    #ifdef UNITY_2021
                    if (klass && il2cpp_functions::class_is_valuetype(klass)) {
                    #else
                    if (klass && klass->valuetype) {
                    #endif
                        // Arg is an Il2CppObject* of a value type. It needs to be unboxed.
                        return il2cpp_functions::object_unbox(reinterpret_cast<Il2CppObject*>(arg));
                    }
                }
            }
            return arg;
        } else if constexpr (has_il2cpp_conversion<Dt>) {
            return arg.convert();
        }
        else {
            return const_cast<Dt*>(&arg);
        }
    }

    template<class T>
    void* ExtractTypeValue(T& arg) {
        using Dt = ::std::decay_t<T>;
        if constexpr (std::is_same_v<nullptr_t, T>) {
            return nullptr;
        }
        else if constexpr (has_il2cpp_conversion<T>) {
            return arg.convert();
        }
        else if constexpr (::std::is_pointer_v<Dt>) {
            // Pointer type, grab class and perform deduction for unbox.
            // Must be classof deducible!
            auto* k = classof(Dt);
            if (k && il2cpp_functions::class_is_valuetype(k)) {
                // Arg is an Il2CppObject* of a value type. It needs to be unboxed.
                return il2cpp_functions::object_unbox(reinterpret_cast<Il2CppObject*>(arg));
            }
            return arg;
        }
        else {
            return const_cast<Dt*>(&arg);
        }
    }

    inline auto ExtractValues() {
        return ::std::array<void*, 0>();
    }

    template <class... TArgs>
    inline auto ExtractValues(TArgs&&... args) {
        constexpr std::size_t array_count = sizeof...(TArgs);
        return std::array<void*, array_count>(::il2cpp_utils::ExtractValue(args)...);
    }

    #if __has_feature(cxx_exceptions)
    /// @brief Instantiates a generic MethodInfo* from the provided Il2CppClasses.
    /// This method will throw an Il2CppUtilException if it fails for any reason.
    /// @return MethodInfo* for RunMethod calls.
    const MethodInfo* MakeGenericMethod(const MethodInfo* info, ::std::span<Il2CppClass*> const types);
    /// @brief Finds the first MethodInfo* described by the given Il2CppClass*, method name, and argument count.
    /// Throws an Il2CppUtilException when: klass is null, or the method could not be found.
    /// @return The found MethodInfo*
    /// @param klass The Il2CppClass* to search for the method
    /// @param methodName The il2cpp name of the method to find
    /// @param argsCount The number of arguments to match (or -1 to not match at all)
    const MethodInfo* FindMethodUnsafe(const Il2CppClass* klass, ::std::string_view methodName, int argsCount);
    /// @brief Find the first MethodInfo* on the given instance, described by the methodName, and argument count.
    /// Throws an Il2CppUtilException when: instance is null, the Il2CppClass* could not be loaded, or the method could not be found.
    /// @return The found MethodInfo*
    /// @param instance The Il2CppObject* to search for the method
    /// @param methodName The il2cpp name of the method to find
    /// @param argsCount The number of arguments to match (or -1 to not match at all)
    const MethodInfo* FindMethodUnsafe(Il2CppObject* instance, ::std::string_view methodName, int argsCount);
    /// @brief Find the first MethodInfo* of the class described by the namespace and className, described by the methodName, and argument count.
    /// Throws an Il2CppUtilException when: the Il2CppClass* could not be found, or the method could not be found.
    /// @return The found MethodInfo*
    /// @param nameSpace The namespace in which to search for the class
    /// @param className The il2cpp name of the class to find
    /// @param methodName The il2cpp name of the method to find
    /// @param argsCount The number of arguments to match (or -1 to not match at all)
    const MethodInfo* FindMethodUnsafe(::std::string_view nameSpace, ::std::string_view className, ::std::string_view methodName, int argsCount);
    const MethodInfo* FindMethod(FindMethodInfo& info);
    #ifndef BS_HOOK_USE_CONCEPTS
    template <typename... TArgs, ::std::enable_if_t<(... && !::std::is_convertible_v<TArgs, FindMethodInfo>), int> = 0>
    #else
    template<typename... TArgs>
    requires (... && !::std::is_convertible_v<TArgs, FindMethodInfo>)
    #endif
    const MethodInfo* FindMethod(TArgs&&... args) {
        auto info = FindMethodInfo(args...);
        return FindMethod(info);
    }

    bool IsConvertibleFrom(const Il2CppType* to, const Il2CppType* from, bool asArgs = true);

    // Returns if a given MethodInfo's parameters match the Il2CppType vector
    template<size_t genSz, size_t argSz>
    bool ParameterMatch(const MethodInfo* method, std::span<const Il2CppClass*, genSz> const genTypes, std::span<const Il2CppType*, argSz> const argTypes) {
        static auto logger = getLogger().WithContext("ParameterMatch");
        il2cpp_functions::Init();
        if (method->parameters_count != argTypes.size()) {
            logger.warning("Potential method match had wrong number of parameters %i (expected %lu)", method->parameters_count, argTypes.size());
            return false;
        }

        const Il2CppGenericContainer* genContainer;

        int32_t genCount = 0;
        if (method->is_generic) {
            if (method->is_inflated) {
                auto genMethodInfo = method->genericMethod;
#ifdef UNITY_2021
                genContainer = reinterpret_cast<const Il2CppGenericContainer*>(genMethodInfo->methodDefinition->genericContainerHandle);
#else
                genContainer = genMethodInfo->methodDefinition->genericContainerHandle;
#endif
            } else {
#ifdef UNITY_2021
                genContainer = reinterpret_cast<const Il2CppGenericContainer*>(method->genericContainerHandle);
#else
                genContainer = method->genericContainer;
#endif
            }
            genCount = genContainer->type_argc;
        }

        if ((size_t)genCount != genTypes.size()) {
            logger.warning("Potential method match had wrong number of generics %i (expected %lu)", genCount, genTypes.size());
            logger.warning("is generic %i is inflated %i", method->is_generic, method->is_inflated);
            return false;
        }
        // TODO: supply boolStrictMatch and use type_equals instead of IsConvertibleFrom if supplied?
        for (decltype(method->parameters_count) i = 0; i < method->parameters_count; i++) {
            auto* paramType = method->parameters[i];
            if (paramType->type == IL2CPP_TYPE_MVAR) {
                if (genCount == 0) {
                    logger.warning("No generic args to extract paramIdx %i", i);
                    continue;
                }
                auto genIdx = il2cpp_functions::MetadataCache_GetGenericParameterIndexFromParameter(paramType->data.genericParameterHandle) - genContainer->genericParameterStart;
                if (genIdx < 0) {
                    logger.warning("Extracted invalid genIdx %i from parameter %i", genIdx, i);
                    continue;
                }
                if (genIdx >= genCount) {
                    logger.warning(
                        "ParameterMatch was not supplied enough genTypes to determine type of parameter %i "
                        "(had %i, needed %i)!",
                        i, genCount, genIdx);
                    continue;
                }

                auto* klass = genTypes[genIdx];
                paramType = (paramType->byref) ? &klass->this_arg : &klass->byval_arg;
            }
            // TODO: just because two parameter lists match doesn't necessarily mean this is the best match...
            if (!IsConvertibleFrom(paramType, argTypes[i])) {
                return false;
            }
        }
        return true;
    }

    template <size_t argSz>
    bool ParameterMatch(const MethodInfo* method, ::std::span<const Il2CppType*, argSz> const argTypes) {
        std::array<const Il2CppClass*, 0> gens;
        return ParameterMatch<0, argSz>(method, std::span(gens), argTypes);
    }

    template <size_t genSz, size_t argSz>
    bool ParameterMatch(const MethodInfo* method, std::array<const Il2CppClass*, genSz> const& genTypes, std::array<const Il2CppType*, argSz> const& argTypes) {
        return ParameterMatch<genSz, argSz>(method, genTypes, argTypes);
    }

    template <size_t sz>
    bool ParameterMatch(const MethodInfo* method, std::array<const Il2CppType*, sz> const& argTypes) {
        std::array<const Il2CppClass*, 0> gens;
        return ParameterMatch<0, sz>(method, gens, argTypes);
    }

    /// @brief Calls the methodPointer on the provided const MethodInfo*, but throws a RunMethodException on failure.
    /// If checkTypes is false, does not perform type checking and instead is a partially unsafe wrapper around invoking the methodPointer directly.
    /// This function still performs simple checks (such as void vs. non-void returns and instance vs. static method invokes) even with checkTypes as false.
    /// If you wish to forward this call to runtime_invoke (for example, in order to catch exceptions), consider using RunMethod/RunMethodUnsafe instead.
    /// @tparam TOut The output to return. Defaults to void.
    /// @tparam checkTypes Whether to check types or not. Defaults to true.
    /// @tparam T The instance type.
    /// @tparam TArgs The argument types.
    /// @param instance The instance to invoke with. Should almost always be `this`.
    /// @param method The MethodInfo* to use for type checking and conversions.
    /// @param mPtr The method pointer to invoke specifically.
    /// @param params The arguments to pass into the function.
    template<class TOut = void, bool checkTypes = true, class T, class... TArgs>
    TOut RunMethodThrow(T* instance, const MethodInfo* method, Il2CppMethodPointer mPtr, TArgs&&... params) {
        static auto& logger = getLogger();
        if (!method) {
            throw RunMethodException("Method cannot be null!", nullptr);
        }
        if (!mPtr) {
            throw RunMethodException("Method pointer cannot be null (don't call an abstract method directly!)", method);
        }

        if constexpr (checkTypes && sizeof...(TArgs) > 0) {
            std::array<const Il2CppType*, sizeof...(TArgs)> types{::il2cpp_utils::ExtractType(params)...};
            if (!ParameterMatch(method, types)) {
                throw RunMethodException("Parameters do not match!", method);
            }
            auto* outType = ExtractIndependentType<TOut>();
            if (outType) {
                if (!IsConvertibleFrom(outType, method->return_type, false)) {
                    logger.warning("User requested TOut %s does not match the method's return object of type %s!",
                        TypeGetSimpleName(outType), TypeGetSimpleName(method->return_type));
                    throw RunMethodException("Return type of method is not convertible!", method);
                }
            }
        }
        // NOTE: We need to remove references from our method pointers and copy in our parameters
        // This works great for all cases EXCEPT for byref types
        // For byref types, because we copy in our parameters, we need to provide a wrapper type that wraps a reference
        // That type then is provided and copied in.
        // That type is in byref.hpp as ByRef(T&&)

        // Need to potentially call Class::Init here as well
        // This snippet is almost identical to what libil2cpp does
        if ((method->flags & METHOD_ATTRIBUTE_STATIC) > 0 && method->klass && !method->klass->cctor_finished_or_no_cctor) {
            il2cpp_functions::Class_Init(method->klass);
        }
        try {
            if constexpr (std::is_same_v<TOut, void>) {
                // Method has void return
                if (!il2cpp_functions::type_equals(method->return_type, &il2cpp_functions::defaults->void_class->byval_arg)) {
                    // If the method does NOT have a void return, yet we asked for one, this fails.
                    // This should ALWAYS fail because it's very wrong, regardless of checkTypes.
                    throw RunMethodException("Return type of method is not void, yet was requested as void!", method);
                }
                if ((method->flags & METHOD_ATTRIBUTE_STATIC) > 0) {
                    // Static method
                    reinterpret_cast<void (*)(std::remove_reference_t<TArgs>..., const MethodInfo*)>(mPtr)(params..., method);
                } else {
                    if (il2cpp_functions::class_is_valuetype(method->klass)) {
                        // Value type instance method. Instance parameter is always boxed in some way.
                        auto boxedRepr = instance;
                        if constexpr (sizeof(Il2CppCodeGenModule) <= 104) {
                            // Boxing is only required if we invoke to adjustor thunks instead of actual impls
                            // Note that for whatever reason, we have exposed methods that are compiled that use literals, yet we still need to passed boxed reprs.
                            if constexpr (il2cpp_type_check::need_box<T>::value) {
                                // TODO: Eventually remove this dependence on il2cpp_functions::Init
                                il2cpp_functions::Init();
                                // Yeah, we cast literally all over the place.
                                boxedRepr = reinterpret_cast<T*>(il2cpp_functions::value_box(classof(T), boxedRepr));
                            } else {
                                boxedRepr = reinterpret_cast<T*>(reinterpret_cast<void**>(boxedRepr) - 2);
                            }
                        }
                        reinterpret_cast<void (*)(T*, std::remove_reference_t<TArgs>..., const MethodInfo*)>(mPtr)(boxedRepr, params..., method);
                        if constexpr (sizeof(Il2CppCodeGenModule) <= 104) {
                            *instance = *reinterpret_cast<T*>(il2cpp_functions::object_unbox(reinterpret_cast<Il2CppObject*>(boxedRepr)));
                        }
                    } else {
                        reinterpret_cast<void (*)(T*, std::remove_reference_t<TArgs>..., const MethodInfo*)>(mPtr)(instance, params..., method);
                    }
                }
            } else {
                // Method has non-void return
                // if (il2cpp_functions::class_from_type(method->return_type)->instance_size != sizeof(TOut)) {
                    // TODO:
                    // The return value's size must always match. We know TOut is not void, but we do not know of anything else
                    // If the return value of the method is of a different size than TOut we should throw.
                    // Note that we cannot simply check sizeof(TOut) and compare it to instance size, since a TOut pointer would not match.
                    // We would need to properly ensure that the type is either byval or this_arg before comparing and/or ensuring size match
                // }
                // As a simple check, we can make sure the method we are attempting to call is not a void method:
                if (il2cpp_functions::type_equals(method->return_type, &il2cpp_functions::defaults->void_class->byval_arg)) {
                    throw RunMethodException("Return type of method is void, yet was requested as non-void!", method);
                }
                if ((method->flags & METHOD_ATTRIBUTE_STATIC) > 0) {
                    // Static method
                    return reinterpret_cast<TOut (*)(std::remove_reference_t<TArgs>..., const MethodInfo*)>(mPtr)(params..., method);
                } else {
                    if (il2cpp_functions::class_is_valuetype(method->klass)) {
                        auto boxedRepr = instance;
                        if constexpr (sizeof(Il2CppCodeGenModule) <= 104) {
                            // Boxing is only required if we invoke to adjustor thunks instead of actual impls
                            // Note that for whatever reason, we have exposed methods that are compiled that use literals, yet we still need to passed boxed reprs.
                            if constexpr (il2cpp_type_check::need_box<T>::value) {
                                // TODO: Eventually remove this dependence on il2cpp_functions::Init
                                il2cpp_functions::Init();
                                // Yeah, we cast literally all over the place.
                                boxedRepr = reinterpret_cast<T*>(il2cpp_functions::value_box(classof(T), boxedRepr));
                            } else {
                                boxedRepr = reinterpret_cast<T*>(reinterpret_cast<void**>(boxedRepr) - 2);
                            }
                        }
                        TOut res = reinterpret_cast<TOut (*)(T*, std::remove_reference_t<TArgs>..., const MethodInfo*)>(mPtr)(boxedRepr, params..., method);
                        if constexpr (sizeof(Il2CppCodeGenModule) <= 104) {
                            *instance = *reinterpret_cast<T*>(il2cpp_functions::object_unbox(reinterpret_cast<Il2CppObject*>(boxedRepr)));
                        }
                        return res;
                    } else {
                        return reinterpret_cast<TOut (*)(T*, std::remove_reference_t<TArgs>..., const MethodInfo*)>(mPtr)(instance, params..., method);
                    }
                }
            }
        } catch (Il2CppExceptionWrapper& wrapper) {
            logger.error("%s: Failed with exception: %s", il2cpp_functions::method_get_name(method),
                il2cpp_utils::ExceptionToString(wrapper.ex).c_str());
            throw RunMethodException(wrapper.ex, method);
        }
    }

    /// @brief Calls the methodPointer on the provided const MethodInfo*, but throws a RunMethodException on failure.
    /// If checkTypes is false, does not perform type checking and instead is a partially unsafe wrapper around invoking the methodPointer directly.
    /// This function still performs simple checks (such as void vs. non-void returns and instance vs. static method invokes) even with checkTypes as false.
    /// @tparam TOut The output to return. Defaults to void.
    /// @tparam checkTypes Whether to check types or not. Defaults to true.
    /// @tparam T The instance type (either an actual instance or an Il2CppClass*/Il2CppType*).
    /// @tparam TArgs The argument types.
    /// @param instance The instance or Il2CppClass*/Il2CppType* to invoke with.
    /// @param method The MethodInfo* to invoke.
    /// @param params The arguments to pass into the function.
    template<class TOut = void, bool checkTypes = true, class T, class... TArgs>
    TOut RunMethodThrow(T* instance, const MethodInfo* method, TArgs&& ...params) {
        return RunMethodThrow<TOut, checkTypes>(instance, method, method->methodPointer, params...);
    }

    /// @brief Runs the provided method and rethrows any exception that occurs. Will throw a RunMethodException.
    /// If checkTypes is false, does not perform type checking and instead is an unsafe wrapper around runtime_invoke.
    /// @tparam TOut The output to return. Defaults to void.
    /// @tparam checkTypes Whether to check types or not. Defaults to true.
    /// @tparam T The instance type (an actual instance or nullptr Il2CppClass*, etc.)
    /// @tparam TArgs The argument types.
    /// @param instance The instance or nullptr Il2CppClass* to invoke with.
    /// @param method The MethodInfo* to invoke.
    /// @param params The arguments to pass into the function.
    /// @return The result from the function, or will throw.
    template<class TOut = void, bool checkTypes = true, class T, class... TArgs>
    TOut RunMethodRethrow(T&& instance, const MethodInfo* method, TArgs&& ...params) {
        static auto& logger = getLogger();
        CRASH_UNLESS(method);

        if constexpr (checkTypes) {
            // only check args if TArgs is > 0
            if constexpr (sizeof...(TArgs) > 0) {
                std::array<const Il2CppType*, sizeof...(TArgs)> types{ ::il2cpp_utils::ExtractType(params)... };
                if (!ParameterMatch(method, types)) {
                    throw RunMethodException("Parameters do not match!", method);
                }
            }

            if constexpr (!std::is_same_v<TOut, void>) {
                auto* outType = ExtractIndependentType<TOut>();
                if (outType) {
                    if (!IsConvertibleFrom(outType, method->return_type, false)) {
                        logger.warning("User requested TOut %s does not match the method's return object of type %s!", TypeGetSimpleName(outType), TypeGetSimpleName(method->return_type));
                        throw RunMethodException(string_format("Return type of method is not convertible to: %s!", TypeGetSimpleName(outType)), method);
                    }
                }
            }
        }

        void* inst = ::il2cpp_utils::ExtractValue(instance);  // null is allowed (for T = Il2CppType* or Il2CppClass*)
        Il2CppException* exp = nullptr;
        std::array<void*, sizeof...(params)> invokeParams{::il2cpp_utils::ExtractTypeValue(params)...};
        il2cpp_functions::Init();
        auto* ret = il2cpp_functions::runtime_invoke(method, inst, invokeParams.data(), &exp);

        if (exp) {
            throw RunMethodException(exp, method);
        }

        if constexpr (!std::is_same_v<TOut, void>) {
            auto re = FromIl2CppObject<TOut>(ret);
            // Return type must be convertible from an Il2CppObject to TOut!
            CRASH_UNLESS(re);
            return *re;
        }
    }
    #else
    /// @brief Instantiates a generic MethodInfo* from the provided Il2CppClasses.
    /// @return MethodInfo* for RunMethod calls, will be nullptr on failure
    const MethodInfo* MakeGenericMethod(const MethodInfo* info, ::std::span<Il2CppClass*> types) noexcept;
    const MethodInfo* FindMethodUnsafe(const Il2CppClass* klass, ::std::string_view methodName, int argsCount) noexcept;
    const MethodInfo* FindMethodUnsafe(Il2CppObject* instance, ::std::string_view methodName, int argsCount) noexcept;
    const MethodInfo* FindMethodUnsafe(::std::string_view nameSpace, ::std::string_view className, ::std::string_view methodName, int argsCount) noexcept;
    const MethodInfo* FindMethod(FindMethodInfo& info) noexcept;
    #ifndef BS_HOOK_USE_CONCEPTS
    template <typename... TArgs, ::std::enable_if_t<(... && !::std::is_convertible_v<TArgs, FindMethodInfo>), int> = 0>
    #else
    template<typename... TArgs>
    requires (... && !::std::is_convertible_v<TArgs, FindMethodInfo>)
    #endif
    const MethodInfo* FindMethod(TArgs&&... args) noexcept {
        auto info = FindMethodInfo(args...);
        return FindMethod(info);
    }

    bool IsConvertibleFrom(const Il2CppType* to, const Il2CppType* from, bool asArgs = true);
    // Returns if a given MethodInfo's parameters match the Il2CppType vector
    bool ParameterMatch(const MethodInfo* method, ::std::span<const Il2CppType*> argTypes);

    // Returns if a given MethodInfo's parameters match the Il2CppType vector and generic types vector
    bool ParameterMatch(const MethodInfo* method, ::std::span<Il2CppClass const*> genTypes, ::std::span<const Il2CppType*> argTypes);
    #endif

    // Function made by zoller27osu, modified by Sc2ad
    // Logs information about the given MethodInfo* as log(DEBUG)
    void LogMethod(LoggerContextObject& logger, const MethodInfo* method);

    // Created by zoller27osu
    // Calls LogMethod on all methods in the given class
    void LogMethods(LoggerContextObject& logger, Il2CppClass* klass, bool logParents = false);

    template<class TOut = Il2CppObject*, bool checkTypes = true, class T, class... TArgs>
    // Runs a MethodInfo with the specified parameters and instance, with return type TOut.
    // Assumes a static method if instance == nullptr. May fail due to exception or wrong name, hence the ::std::optional.
    ::std::optional<TOut> RunMethod(T&& instance, const MethodInfo* method, TArgs&& ...params) {
        static auto& logger = getLogger();
        RET_NULLOPT_UNLESS(logger, method);

        if constexpr (checkTypes && sizeof...(TArgs) > 0) {
            std::array<const Il2CppType*, sizeof...(TArgs)> types{::il2cpp_utils::ExtractType(params)...};
            RET_NULLOPT_UNLESS(logger, ParameterMatch(method, types));
        }

        void* inst = ::il2cpp_utils::ExtractValue(instance);  // null is allowed (for T = Il2CppType* or Il2CppClass*)
        Il2CppException* exp = nullptr;
        std::array<void*, sizeof...(params)> invokeParams{::il2cpp_utils::ExtractValue(params)...};
        il2cpp_functions::Init();
        auto* ret = il2cpp_functions::runtime_invoke(method, inst, invokeParams.data(), &exp);

        // Check if the TOut that the user requested makes sense given the Il2CppObject* we actually got
        if constexpr (checkTypes) {
            if (ret) {
                // By using this instead of ExtractType, we avoid unboxing because the ultimate type in that case would depend on the
                // method in the first place
                auto* outType = ExtractIndependentType<TOut>();
                if (outType) {
                    auto* retType = ExtractType(ret);
                    if (!IsConvertibleFrom(outType, retType, false)) {
                        logger.warning("User requested TOut %s does not match the method's return object of type %s!",
                            TypeGetSimpleName(outType), TypeGetSimpleName(retType));
                    }
                }
            }
        }

        if (exp) {
            logger.error("%s: Failed with exception: %s", il2cpp_functions::method_get_name(method),
                il2cpp_utils::ExceptionToString(exp).c_str());
            return ::std::nullopt;
        }

        return FromIl2CppObject<TOut>(ret);
    }

    template<class TOut = Il2CppObject*, bool checkTypes = true, class... TArgs>
    // Simply forwards arguments to RunMethod<TOut, checkTypes>(static_cast<Il2CppClass*>(nullptr), ...)
    ::std::optional<TOut> RunStaticMethod(const MethodInfo* method, TArgs&& ...params) {
        return RunMethod<TOut, checkTypes>(static_cast<Il2CppClass*>(nullptr), method, params...);
    }

    template<class TOut = Il2CppObject*, class T, class... TArgs>
    // Simply forwards arguments to RunMethod<TOut, false>
    ::std::optional<TOut> RunMethodUnsafe(T&& instance, const MethodInfo* method, TArgs&& ...params) {
        return RunMethod<TOut, false>(instance, method, params...);
    }

    template<class TOut = Il2CppObject*, class... TArgs>
    // Simply forwards arguments to RunMethod<TOut, false>(static_cast<Il2CppClass*>(nullptr), ...)
    ::std::optional<TOut> RunStaticMethodUnsafe(const MethodInfo* method, TArgs&& ...params) {
        return RunMethod<TOut, false>(static_cast<Il2CppClass*>(nullptr), method, params...);
    }

    template<class TOut = Il2CppObject*, bool checkTypes = true, class T, class... TArgs>
    // Runs a (static) method with the specified method name, with return type TOut.
    // Checks the types of the parameters against the candidate methods.
    #ifndef BS_HOOK_USE_CONCEPTS
    ::std::enable_if_t<!::std::is_convertible_v<T, ::std::string_view>, ::std::optional<TOut>>
    #else
    requires (!::std::is_convertible_v<T, ::std::string_view>) ::std::optional<TOut>
    #endif
    RunMethod(T&& classOrInstance, ::std::string_view methodName, TArgs&& ...params) {
        static auto& logger = getLogger();
        if constexpr (checkTypes) {
            std::array<const Il2CppType*, sizeof...(TArgs)> types{::il2cpp_utils::ExtractType(params)...};
            auto* method = RET_NULLOPT_UNLESS(logger, FindMethod(classOrInstance, NoArgClass<TOut>(), methodName, types));
            return RunMethod<TOut, true>(classOrInstance, method, params...);
        }
        // TODO: We should probably change how FindMethod is called/isn't called
        // At the moment, however, if you call RunMethod with checkTypes = false, it will perform an FindMethodUnsafe instead
        // But it will return nullptr if there are multiple matches
        else {
            if constexpr (std::is_same_v<T, Il2CppClass*>) {
                auto* method = RET_NULLOPT_UNLESS(logger, FindMethodUnsafe(classOrInstance, methodName, sizeof...(TArgs)));
                return RunMethod<TOut, false>(classOrInstance, method, params...);
            } else {
                auto* klass = RET_NULLOPT_UNLESS(logger, ExtractClass(classOrInstance));
                auto* method = RET_NULLOPT_UNLESS(logger, FindMethodUnsafe(klass, methodName, sizeof...(TArgs)));
                return RunMethod<TOut, false>(classOrInstance, method, params...);
            }
        }
    }

    template<class TOut = Il2CppObject*, bool checkTypes = true, class... TArgs>
    // Runs a static method with the specified method name and arguments, on the class with the specified namespace and class name.
    // The method also has return type TOut.
    ::std::optional<TOut> RunMethod(::std::string_view nameSpace, ::std::string_view klassName, ::std::string_view methodName, TArgs&& ...params) {
        static auto& logger = getLogger();
        auto* klass = RET_NULLOPT_UNLESS(logger, GetClassFromName(nameSpace, klassName));
        return RunMethod<TOut, checkTypes>(klass, methodName, params...);
    }

    /// @brief Instantiates a generic MethodInfo* from the provided Il2CppClasses and invokes it.
    /// @n This method will not crash.
    /// @tparam TOut The return type of the method to invoke
    /// @tparam T Instance type
    /// @tparam TArgs Parameters to RunMethod
    /// @param instance Instance to RunMethod, or null/class
    /// @param info Generic MethodInfo* to invoke
    /// @param genTypes Types to instantiate the generic MethodInfo* with
    /// @param params Parameters to RunMethod
    template<class TOut = Il2CppObject*, class T, class... TArgs>
    ::std::optional<TOut> RunGenericMethod(T&& instance, const MethodInfo* info, ::std::span<Il2CppClass*> genTypes, TArgs&& ...params) noexcept {
        static auto& logger = getLogger();
        auto* createdMethod = RET_NULLOPT_UNLESS(logger, MakeGenericMethod(info, genTypes));
        return RunMethod<TOut, false>(instance, createdMethod, params...);
    }

    template<class TOut = Il2CppObject*, class T, class... TArgs>
    ::std::optional<TOut> RunGenericMethod(T&& classOrInstance, ::std::string_view methodName, ::std::span<Il2CppClass*> genTypes, TArgs&& ...params) noexcept {
        static auto& logger = getLogger();
        std::array<const Il2CppType*, sizeof...(TArgs)> types{::il2cpp_utils::ExtractType(params)...};

        auto* info = RET_NULLOPT_UNLESS(logger, FindMethod(classOrInstance, NoArgClass<TOut>(), methodName, genTypes, types));
        return RunGenericMethod<TOut>(classOrInstance, info, genTypes, params...);
    }
    template<class TOut = Il2CppObject*, class... TArgs>
    // Runs a static generic method with the specified method name and arguments, on the class with the specified namespace and class name.
    // The method also has return type TOut.
    ::std::optional<TOut> RunGenericMethod(::std::string_view nameSpace, ::std::string_view klassName, ::std::string_view methodName, ::std::span<Il2CppClass*> genTypes, TArgs&& ...params) noexcept {
        static auto& logger = getLogger();
        auto* klass = RET_NULLOPT_UNLESS(logger, GetClassFromName(nameSpace, klassName));
        return RunGenericMethod<TOut>(klass, methodName, genTypes, params...);
    }

    template<class TOut = Il2CppObject*, class T, class... TArgs>
    // Runs a (static) method with the specified method name and number of arguments, with return type TOut.
    // DOES NOT PERFORM TYPE CHECKING.
    #ifndef BS_HOOK_USE_CONCEPTS
    ::std::enable_if_t<::std::is_base_of_v<Il2CppClass, T> || ::std::is_base_of_v<Il2CppObject, T>, ::std::optional<TOut>>
    #else
    requires (::std::is_base_of_v<Il2CppClass, T> || ::std::is_base_of_v<Il2CppObject, T>) ::std::optional<TOut>
    #endif
    RunMethodUnsafe(T* classOrInstance, ::std::string_view methodName, TArgs&& ...params) {
        static auto& logger = getLogger();
        RET_NULLOPT_UNLESS(logger, classOrInstance);
        auto* method = RET_NULLOPT_UNLESS(logger, FindMethodUnsafe(classOrInstance, methodName, sizeof...(TArgs)));
        return RunMethod<TOut, false>(classOrInstance, method, params...);
    }

    template<class TOut = Il2CppObject*, class... TArgs>
    // Runs a static method with the specified method name and arguments, on the class with the specified namespace and class name.
    // The method also has return type TOut.
    // DOES NOT PERFORM TYPE CHECKING.
    ::std::optional<TOut> RunMethodUnsafe(::std::string_view nameSpace, ::std::string_view klassName, ::std::string_view methodName, TArgs&& ...params) {
        static auto& logger = getLogger();
        auto* klass = RET_NULLOPT_UNLESS(logger, GetClassFromName(nameSpace, klassName));
        return RunMethodUnsafe<TOut>(klass, methodName, params...);
    }

    template<typename TOut = Il2CppObject*, CreationType creationType = CreationType::Temporary, typename... TArgs>
    // Creates a new object of the given class using the given constructor parameters
    // Will only run a .ctor whose parameter types match the given arguments.
    ::std::optional<TOut> New(Il2CppClass* klass, TArgs&& ...args) {
        static auto& logger = getLogger();
        il2cpp_functions::Init();

        Il2CppObject* obj;
        if constexpr (creationType == CreationType::Temporary) {
            // object_new call
            obj = RET_NULLOPT_UNLESS(logger, il2cpp_functions::object_new(klass));
        } else {
            obj = RET_NULLOPT_UNLESS(logger, createManual(klass));
        }
        // runtime_invoke constructor with right type(s) of arguments, return null if constructor errors
        std::array<const Il2CppType*, sizeof...(TArgs)> types{::il2cpp_utils::ExtractType(args)...};
        auto* method = RET_NULLOPT_UNLESS(logger, FindMethod(klass, ".ctor", types));
        RET_NULLOPT_UNLESS(logger, RunMethod(obj, method, args...));
        return FromIl2CppObject<TOut>(obj);
    }

    // TODO: Rename to New, rename existing New to NewObject or equivalent
    /// @brief Allocates a new instance of a particular Il2CppClass*, either allowing it to be GC'd normally or manually controlled.
    /// The Il2CppClass* is derived from the TOut template parameter.
    /// The found constructor method will be cached.
    /// Will throw either an il2cpp_utils::exceptions::StackTraceException or il2cpp_utils::RunMethodException if errors occur.
    /// @tparam TOut The type to create.
    /// @tparam creationType The way to create the instance.
    /// @tparam TArgs The arguments to call the constructor with.
    /// @param args The arguments to call the constructor with.
    template<class TOut, CreationType creationType = CreationType::Temporary, typename... TArgs>
    TOut NewSpecific(TArgs&&... args) {
        static auto* klass = classof(TOut);
        Il2CppObject* obj;
        if constexpr (creationType == CreationType::Temporary) {
            // object_new call
            obj = il2cpp_functions::object_new(klass);
            if (!obj) {
                throw exceptions::StackTraceException("Failed to allocate new object via object_new!");
            }
        } else {
            obj = createManualThrow(klass);
        }
        // Only need to extract based off of types, since we are asusming our TOut is classof-able already
        static auto ctorMethod = FindMethod(klass, ".ctor", std::array<Il2CppType const*, sizeof...(TArgs)>{ExtractIndependentType<std::remove_reference_t<TArgs>>()...});
        if (!ctorMethod) {
            throw exceptions::StackTraceException(string_format("Failed to find a matching .ctor method during construction of type: %s", ClassStandardName(klass).c_str()));
        }
        ::il2cpp_utils::RunMethodRethrow<void, false>(obj, ctorMethod, args...);
        if constexpr (std::is_pointer_v<TOut>) {
            return reinterpret_cast<TOut>(obj);
        } else if constexpr (has_il2cpp_conversion<TOut>) {
            // Handle construction for wrapper types, construct from void*s
            return TOut(reinterpret_cast<void*>(obj));
        } else {
            static_assert(false_t<TOut>, "Cannot C# construct the provided value type that is not a wrapper type!");
        }
    }

    template<typename TOut = Il2CppObject*, CreationType creationType = CreationType::Temporary, typename... TArgs>
    // Creates a new object of the given class using the given constructor parameters
    // DOES NOT PERFORM ARGUMENT TYPE CHECKING! Uses the first .ctor with the right number of parameters it sees.
    ::std::optional<TOut> NewUnsafe(const Il2CppClass* klass, TArgs&& ...args) {
        static auto& logger = getLogger();
        il2cpp_functions::Init();

        Il2CppObject* obj;
        if constexpr (creationType == CreationType::Temporary) {
            // object_new call
            obj = RET_NULLOPT_UNLESS(logger, il2cpp_functions::object_new(klass));
        } else {
            obj = RET_NULLOPT_UNLESS(logger, createManual(klass));
        }
        // runtime_invoke constructor with right number of args, return null if constructor errors
        RET_NULLOPT_UNLESS(logger, RunMethodUnsafe(obj, ".ctor", args...));
        return FromIl2CppObject<TOut>(obj);
    }

    template<typename TOut = Il2CppObject*, CreationType creationType = CreationType::Temporary, typename... TArgs>
    // Creates a new object of the returned type using the given constructor parameters
    // Will only run a .ctor whose parameter types match the given arguments.
    #ifndef BS_HOOK_USE_CONCEPTS
    ::std::enable_if_t<(... && ((!::std::is_same_v<const Il2CppClass*, TArgs> || !::std::is_same_v<Il2CppClass*, TArgs>) && !::std::is_convertible_v<TArgs, ::std::string_view>)), ::std::optional<TOut>>
    #else
    requires (... && ((!::std::is_same_v<const Il2CppClass*, TArgs> || !::std::is_same_v<Il2CppClass*, TArgs>) && !::std::is_convertible_v<TArgs, ::std::string_view>)) ::std::optional<TOut>
    #endif
    New(TArgs&& ...args) {
        static auto& logger = getLogger();
        auto* klass = RET_NULLOPT_UNLESS(logger, (NoArgClass<TOut, true>()));
        return New<TOut, creationType>(klass, args...);
    }

    template<typename TOut = Il2CppObject*, CreationType creationType = CreationType::Temporary, typename... TArgs>
    // Creates a new object of the class with the given nameSpace and className using the given constructor parameters.
    // Will only run a .ctor whose parameter types match the given arguments.
    ::std::optional<TOut> New(::std::string_view nameSpace, ::std::string_view className, TArgs&& ...args) {
        static auto& logger = getLogger();
        auto* klass = RET_0_UNLESS(logger, GetClassFromName(nameSpace, className));
        return New<TOut, creationType>(klass, args...);
    }

    template<typename TOut = Il2CppObject*, CreationType creationType = CreationType::Temporary, typename... TArgs>
    // Creates a new object of the class with the given nameSpace and className using the given constructor parameters.
    // DOES NOT PERFORM ARGUMENT TYPE CHECKING! Uses the first .ctor with the right number of parameters it sees.
    ::std::optional<TOut> NewUnsafe(::std::string_view nameSpace, ::std::string_view className, TArgs* ...args) {
        static auto& logger = getLogger();
        auto* klass = RET_0_UNLESS(logger, GetClassFromName(nameSpace, className));
        return NewUnsafe<TOut, creationType>(klass, args...);
    }
}

#pragma pack(pop)

#endif
