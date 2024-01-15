#include "../../shared/utils/typedefs.h"
#include "../../shared/utils/il2cpp-utils-methods.hpp"
#include "../../shared/utils/hashing.hpp"
#include "utils/il2cpp-functions.hpp"
#include "utils/il2cpp-utils-classes.hpp"
#include "utils/il2cpp-utils-methods.hpp"
#include <sstream>

namespace std {
    // From https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
    template<class T> void hash_combine(size_t& seed, T v) {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    // Let a "sequence" type be any type that supports .size() and iteration and whose elements are hashable and support !=.
    // Calculates a hash for a sequence.
    template<class T> std::size_t hash_seq(T const& seq) noexcept {
        std::size_t seed = seq.size();
        for(auto i: seq) {
            hash_combine(seed, i);
        }
        return seed;
    }

    // Specializes std::hash for std::vector
    template<class T> struct hash<std::vector<T>> {
        std::size_t operator()(std::vector<T> const& seq) const noexcept {
            return hash_seq(seq);
        }
    };
    // Specializes std::hash for std::vector
    template <>
    struct hash<il2cpp_utils::FindMethodInfo> {
        std::size_t operator()(il2cpp_utils::FindMethodInfo const& info) const noexcept {
            auto hashPtr = std::hash<void*>{};

            auto hashSeqClass = std::hash<vector<const Il2CppClass*>>{};
            auto hashSeqType = std::hash<vector<const Il2CppType*>>{};

            auto hashStr = std::hash<std::string_view>{};

            return hashPtr(info.klass) ^ hashPtr(info.returnType) ^ hashStr(info.name) ^ hashSeqType(info.argTypes) ^ hashSeqClass(info.genTypes);
        }
    };
}


namespace il2cpp_utils {
    typedef std::pair<std::string, std::vector<const Il2CppType*>> classesNamesTypesInnerPairType;
    static std::unordered_map<std::pair<const Il2CppClass*, std::pair<std::string, decltype(MethodInfo::parameters_count)>>, const MethodInfo*, hash_pair_3> classesNamesToMethodsCache;
    static std::unordered_map<FindMethodInfo, const MethodInfo*> classesNamesTypesToMethodsCache;
    std::mutex classNamesMethodsLock;
    std::mutex classTypesMethodsLock;



#if __has_feature(cxx_exceptions)
    const MethodInfo* MakeGenericMethod(const MethodInfo* info, std::span<Il2CppClass*> const types)
    #else
    const MethodInfo* MakeGenericMethod(const MethodInfo* info, std::span<Il2CppClass*> const types) noexcept
    #endif
    {
        static auto logger = getLogger().WithContext("MakeGenericMethod");
        il2cpp_functions::Init();
        // Ensure it exists and is generic
        THROW_OR_RET_NULL(logger, info);
        THROW_OR_RET_NULL(logger, il2cpp_functions::method_is_generic(info));
        static auto* typeClass = THROW_OR_RET_NULL(logger, il2cpp_functions::defaults->systemtype_class);
        // Create the Il2CppReflectionMethod* from the MethodInfo* using the MethodInfo's type
        auto* infoObj = il2cpp_functions::method_get_object(info, nullptr);
        if (!infoObj) {
            logger.error("Failed to get MonoMethod from MethodInfo: %p", info);
            THROW_OR_RET_NULL(logger, infoObj);
        }
        // Populate generic parameters into array
        auto* arr = reinterpret_cast<Array<Il2CppReflectionType*>*>(il2cpp_functions::array_new(typeClass, types.size()));
        if (!arr) {
            logger.error("Failed to create array of length: %lu", types.size());
            THROW_OR_RET_NULL(logger, arr);
        }
        int i = 0;
        for (auto* klass : types) {
            auto* typeObj = GetSystemType(klass);
            if (!typeObj) {
                logger.error("Failed to get type object from class: %s", il2cpp_functions::class_get_name_const(klass));
                THROW_OR_RET_NULL(logger, typeObj);
            }
            arr->values[i] = typeObj;
            i++;
        }
        // Call instance function on infoObj to MakeGeneric
        // Does not need to perform type checking, since if this does not match, it will fail more miserably.
        auto res = il2cpp_utils::RunMethod<Il2CppReflectionMethod*, false>(infoObj, "MakeGenericMethod", arr);
        const auto* returnedInfoObj = RET_0_UNLESS(logger, res);
        if (!returnedInfoObj) {
            logger.error("Failed to get Il2CppReflectionMethod from MakeGenericMethod!");
            THROW_OR_RET_NULL(logger, returnedInfoObj);
        }
        // Get MethodInfo* back from generic instantiated method
        const auto* inflatedInfo = il2cpp_functions::method_get_from_reflection(returnedInfoObj);
        if (!inflatedInfo) {
            logger.error("Got null MethodInfo* from Il2CppReflectionMethod: %p", returnedInfoObj);
            THROW_OR_RET_NULL(logger, inflatedInfo);
        }
        // Return method to be invoked by caller
        return inflatedInfo;
    }

    const MethodInfo* ResolveMethodWithSlot(Il2CppClass* klass, uint16_t slot) noexcept {
        il2cpp_functions::Init();
        if (!klass->initialized_and_no_error) il2cpp_functions::Class_Init(klass);

        static auto logger = getLogger().WithContext("ResolveMethodWithSlot");

        auto mend = klass->methods + klass->method_count;
        for (auto itr = klass->methods; itr != mend; itr++) {
            if ((*itr)->slot == slot) {
                return *itr;
            }
        }

        return nullptr;
    }

    const MethodInfo* ResolveVtableSlot(Il2CppClass* klass, Il2CppClass* declaringClass, uint16_t slot) noexcept {
        il2cpp_functions::Init();
        if (!klass->initialized_and_no_error) il2cpp_functions::Class_Init(klass);
        if (!declaringClass->initialized_and_no_error) il2cpp_functions::Class_Init(declaringClass);

        static auto logger = getLogger().WithContext("ResolveVtableSlot");
        if(il2cpp_functions::class_is_interface(declaringClass)) {
            // if the declaring class is an interface,
            // vtable_count means nothing and instead method_count should be used
            // their vtables are just as valid though!
            if (slot >= declaringClass->method_count) { // we tried looking for a slot that is outside the bounds of the interface vtable
                // dump some info so the user can know which method was attempted to be resolved
                logger.error("Declaring class has a vtable that's too small, dumping resolve info:");
                logger.error("Instance class:                   %s::%s", klass->namespaze, klass->name);
                logger.error("Instance class vtable slots:      %u", klass->vtable_count);
                logger.error("Declaring class:                  %s::%s", declaringClass->namespaze, declaringClass->name);
                logger.error("Declaring class vtable slots:     %u", declaringClass->vtable_count);
                logger.error("Attempted slot:                   %u", slot);
                return {};
            }

            for (uint16_t i = 0; i < klass->interface_offsets_count; i++) {
                if(klass->interfaceOffsets[i].interfaceType == declaringClass) {
                    int32_t offset = klass->interfaceOffsets[i].offset;
                    RET_DEFAULT_UNLESS(logger, offset + slot < klass->vtable_count);
                    return klass->vtable[offset + slot].method;
                }
            }

            // if klass is an interface itself, and we haven't found the method yet, we should look in klass->methods anyway
            if (il2cpp_functions::class_is_interface(klass)) {
                RET_DEFAULT_UNLESS(logger, slot < klass->method_count);
                return klass->methods[slot];
            }

            logger.error("could not find method in slot %i of interface '%s' in class '%s'!", slot, ClassStandardName(declaringClass).c_str(), ClassStandardName(klass).c_str());
        } else {
            RET_DEFAULT_UNLESS(logger, slot < klass->vtable_count);
            auto method = klass->vtable[slot].method;

            if (method->slot != slot) {
                logger.warning("Resolving vtable slot led to a method info with a different slot! is this method abstract?");
                logger.warning("Looking for: %d, resolved to: %d", slot, method->slot);

                method = ResolveMethodWithSlot(klass, slot);
                logger.info("After resolving method with slot: found method %p", method);

            }

            // resolved method slot should be the slot we asked for if it came from a non-interface
            RET_DEFAULT_UNLESS(logger, method && slot == method->slot);

            return method;
        }
        return nullptr;
    }

    const MethodInfo* ResolveVtableSlot(Il2CppClass* klass, std::string_view declaringNamespace, std::string_view declaringClassName, uint16_t slot) noexcept {
        return ResolveVtableSlot(klass, GetClassFromName(declaringNamespace, declaringClassName), slot);
    }

    #if __has_feature(cxx_exceptions)
    const MethodInfo* FindMethodUnsafe(const Il2CppClass* klass, std::string_view methodName, int argsCount)
    #else
    const MethodInfo* FindMethodUnsafe(const Il2CppClass* klass, std::string_view methodName, int argsCount) noexcept
    #endif
    {
        il2cpp_functions::Init();
        static auto logger = getLogger().WithContext("FindMethodUnsafe");
        RET_DEFAULT_UNLESS(logger, klass);

        // Check Cache
        auto innerPair = std::pair<std::string, decltype(MethodInfo::parameters_count)>(methodName, argsCount);
        auto key = std::pair<const Il2CppClass*, decltype(innerPair)>(klass, innerPair);
        classNamesMethodsLock.lock();
        auto itr = classesNamesToMethodsCache.find(key);
        if (itr != classesNamesToMethodsCache.end()) {
            classNamesMethodsLock.unlock();
            return itr->second;
        }
        classNamesMethodsLock.unlock();
        // Recurses through klass's parents
        auto methodInfo = il2cpp_functions::class_get_method_from_name(klass, methodName.data(), argsCount);
        if (!methodInfo) {
            logger.error("could not find method %s with %i parameters in class '%s'!", methodName.data(), argsCount, ClassStandardName(klass).c_str());
            LogMethods(logger, const_cast<Il2CppClass*>(klass), true);
            RET_DEFAULT_UNLESS(logger, methodInfo);
        }
        classNamesMethodsLock.lock();
        classesNamesToMethodsCache.emplace(key, methodInfo);
        classNamesMethodsLock.unlock();
        return methodInfo;
    }

    #if __has_feature(cxx_exceptions)
    const MethodInfo* FindMethodUnsafe(std::string_view nameSpace, std::string_view className, std::string_view methodName, int argsCount)
    #else
    const MethodInfo* FindMethodUnsafe(std::string_view nameSpace, std::string_view className, std::string_view methodName, int argsCount) noexcept
    #endif
    {
        return FindMethodUnsafe(GetClassFromName(nameSpace, className), methodName, argsCount);
    }

    #if __has_feature(cxx_exceptions)
    const MethodInfo* FindMethodUnsafe(Il2CppObject* instance, std::string_view methodName, int argsCount)
    #else
    const MethodInfo* FindMethodUnsafe(Il2CppObject* instance, std::string_view methodName, int argsCount) noexcept
    #endif
    {
        static auto logger = getLogger().WithContext("FindMethodUnsafe");
        il2cpp_functions::Init();
        auto klass = RET_DEFAULT_UNLESS(logger, il2cpp_functions::object_get_class(instance));
        return FindMethodUnsafe(klass, methodName, argsCount);
    }

    #if __has_feature(cxx_exceptions)
    const MethodInfo* FindMethod(FindMethodInfo& info)
    #else
    const MethodInfo* FindMethod(FindMethodInfo& info) noexcept
    #endif
    {
        static auto logger = getLogger().WithContext("FindMethod");
        il2cpp_functions::Init();
        auto* klass = info.klass;
        RET_DEFAULT_UNLESS(logger, klass);

        // TODO: make cache work for generics (stratify by generics count?) and differing return types?
        // Check Cache
        classTypesMethodsLock.lock();
        auto itr = classesNamesTypesToMethodsCache.find(info);
        if (itr != classesNamesTypesToMethodsCache.end()) {
            classTypesMethodsLock.unlock();
            return itr->second;
        }
        classTypesMethodsLock.unlock();

        void* myIter = nullptr;
        const MethodInfo* methodInfo = nullptr;  // basic match
        bool multipleBasicMatches = false;
        const MethodInfo* returnMatch = nullptr;
        bool multipleReturnMatches = false;
        const MethodInfo* perfectMatch = nullptr;
        bool multiplePerfectMatches = false;
        // Does NOT automatically recurse through klass's parents
        while (const MethodInfo* current = il2cpp_functions::class_get_methods(info.klass, &myIter)) {
            if (info.name != current->name) {
                logger.debug("Method name does not match for method %s", current->name);
                continue;
            }
            if (!ParameterMatch(current, std::span(info.genTypes), std::span(info.argTypes))) {
                logger.debug("Parameters do not match for method %s", current->name);
                continue;
            }
            // check return type
            if (info.returnType) {
                auto* returnClass = il2cpp_functions::class_from_il2cpp_type(current->return_type);
                if (info.returnType == returnClass) {
                    if (perfectMatch) {
                        multiplePerfectMatches = true;
                        logger.error("Multiple perfect matches???");
                    } else
                        perfectMatch = current;
                }
                if (il2cpp_functions::class_is_assignable_from(info.returnType, returnClass)) {
                    if (returnMatch) {
                        logger.debug("Multiple return type matches.");
                        multipleReturnMatches = true;
                    } else
                        returnMatch = current;
                }

                if (!perfectMatch && !returnMatch) {
                    logger.debug("Return type does not match for method %s", current->name);
                }
            }
            if (methodInfo)
                multipleBasicMatches = true;
            else
                methodInfo = current;
        }
        // look in parent
        if (!methodInfo && klass->parent && klass->parent != klass) {
            logger.debug("Method does not exist in %s, looking at parent %s", ClassStandardName(klass).c_str(), ClassStandardName(klass->parent).c_str());

            info.klass = klass->parent;
            methodInfo = FindMethod(info);
            info.klass = klass;
        }

        if (!multiplePerfectMatches && perfectMatch) {
            // It's important that these kinds of matches aren't added to the general cache
            // TODO: Explain why!
            return perfectMatch;
        } else if (!multipleReturnMatches && returnMatch) {
            // TODO: Explain why!
            return returnMatch;
        } else if (!methodInfo || multipleBasicMatches) {
            std::stringstream ss;
            ss << ((multipleBasicMatches) ? "found multiple matches for" : "could not find");
            ss << " method " << info.name;
            bool first = true;
            // start listing generic types
            if (!info.genTypes.empty()) {
                ss << "<";
                for (auto t : info.genTypes) {
                    if (!first) ss << ", ";
                    first = false;
                    ss << ClassStandardName(t);
                }
                ss << ">";
            }

            // start listing params
            first = true;
            ss << "(";
            for (auto t : info.argTypes) {
                if (!first) ss << ", ";
                first = false;
                ss << TypeGetSimpleName(t);
            }
            ss << ") in class '" << ClassStandardName(klass) << "'!";
            logger.error("%s", ss.str().c_str());
            LogMethods(logger, klass);
            RET_DEFAULT_UNLESS(logger, !methodInfo || multipleBasicMatches);
        }
        // cache only if basic match
        classTypesMethodsLock.lock();
        classesNamesTypesToMethodsCache.emplace(info, methodInfo);
        classTypesMethodsLock.unlock();
        return methodInfo;
    }

    void LogMethods(LoggerContextObject& logger, Il2CppClass* klass, bool logParents) {
        RET_V_UNLESS(logger, klass);

        if (klass->name) {
            il2cpp_functions::Init();
            il2cpp_functions::Class_Init(klass);
        }
        if (klass->method_count && !(klass->methods)) {
            logger.warning("Class is valid and claims to have methods but ->methods is null! class name: %s", ClassStandardName(klass).c_str());
            return;
        }
        if (logParents) logger.info("class name: %s", ClassStandardName(klass).c_str());

        logger.debug("method_count: %i", klass->method_count);
        for (int i = 0; i < klass->method_count; i++) {
            if (klass->methods[i]) {
                logger.debug("Method %i:", i);
                LogMethod(logger, klass->methods[i]);
            } else {
                logger.warning("Method: %i Does not exist!", i);
            }
        }
        usleep(100);  // 0.0001s
        if (logParents && klass->parent && (klass->parent != klass)) {
            LogMethods(logger, klass->parent, logParents);
        }
    }

    void LogMethod(LoggerContextObject& logger, const MethodInfo* method) {
        il2cpp_functions::Init();
        RET_V_UNLESS(logger, method);

        auto flags = il2cpp_functions::method_get_flags(method, nullptr);
        std::stringstream flagStream;
        if (flags & METHOD_ATTRIBUTE_STATIC) flagStream << "static ";
        if (flags & METHOD_ATTRIBUTE_VIRTUAL) flagStream << "virtual ";
        if (flags & METHOD_ATTRIBUTE_ABSTRACT) flagStream << "abstract ";
        const auto& flagStrRef = flagStream.str();
        const char* flagStr = flagStrRef.c_str();
        auto* retType = il2cpp_functions::method_get_return_type(method);
        auto retTypeStr = TypeGetSimpleName(retType);
        auto methodName = il2cpp_functions::method_get_name(method);
        methodName = methodName ? methodName : "__noname__";
        std::stringstream paramStream;
        for (size_t i = 0; i < il2cpp_functions::method_get_param_count(method); i++) {
            if (i > 0) paramStream << ", ";
            auto* argType = il2cpp_functions::method_get_param(method, i);
            if (il2cpp_functions::type_is_byref(argType)) {
                paramStream << "out/ref ";
            }
            paramStream << TypeGetSimpleName(argType) << " ";
            auto name = il2cpp_functions::method_get_param_name(method, i);
            paramStream << (name ? name : "__noname__");
        }
        const auto& paramStrRef = paramStream.str();
        const char* paramStr = paramStrRef.c_str();
        // TODO: add <T> after methodName
        logger.debug("%s%s %s(%s);", flagStr, retTypeStr, methodName, paramStr);
    }

    bool IsConvertibleFrom(const Il2CppType* to, const Il2CppType* from, bool asArgs) {
        static auto logger = getLogger().WithContext("IsConvertibleFrom");
        RET_0_UNLESS(logger, to);
        RET_0_UNLESS(logger, from);
        if (asArgs) {
            if (to->byref) {
                if (!from->byref) {
                    logger.debug("to (%s, %p) is ref/out while from (%s, %p) is not. Not convertible.",
                        TypeGetSimpleName(to), to, TypeGetSimpleName(from), from);
                    return false;
                }
            }
        }
        il2cpp_functions::Init();
        auto classTo = il2cpp_functions::class_from_il2cpp_type(to);
        auto classFrom = il2cpp_functions::class_from_il2cpp_type(from);
        bool ret = (to->type == IL2CPP_TYPE_MVAR) || il2cpp_functions::class_is_assignable_from(classTo, classFrom);
        if (!ret) {
            if (il2cpp_functions::class_is_enum(classTo)) {
                ret = IsConvertibleFrom(il2cpp_functions::class_enum_basetype(classTo), from, asArgs);
            }
        }
        return ret;
    }
}
