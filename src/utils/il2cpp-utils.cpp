#include "../../shared/utils/typedefs.h"
#include <utility>  // for std::pair
#include "shared/utils/gc-alloc.hpp"
#include "../../shared/utils/hashing.hpp"
#include "../../shared/utils/il2cpp-utils.hpp"
#include "../../shared/utils/utils.h"
#include "../../shared/utils/il2cpp-functions.hpp"
#include <algorithm>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <vector>

// Please see comments in il2cpp-utils.hpp
// TODO: Make this into a static class
namespace il2cpp_utils {
    std::vector<const Il2CppType*> ClassVecToTypes(std::vector<const Il2CppClass*> seq) {
        il2cpp_functions::Init();

        std::vector<const Il2CppType*> types(seq.size());
        std::transform(seq.begin(), seq.end(), types.begin(), [](const Il2CppClass * klass) {return il2cpp_functions::class_get_type_const(klass);});
        return types;
    }

    const Il2CppType* MakeRef(const Il2CppType* type) {
        if (type->byref) return type;
        il2cpp_functions::Init();
        // could use Class::GetByrefType instead of &->this_arg but it does the same thing
        return &il2cpp_functions::class_from_il2cpp_type(type)->this_arg;
    }

    const Il2CppType* UnRef(const Il2CppType* type) {
        if (!type->byref) return type;
        il2cpp_functions::Init();
        return il2cpp_functions::class_get_type(il2cpp_functions::class_from_il2cpp_type(type));
    }



    static std::unordered_map<Il2CppClass*, const char*> typeMap;
    static std::mutex typeLock;

    // TODO: somehow append "out/ref " to the front if type->byref? Make a TypeStandardName?
    const char* TypeGetSimpleName(const Il2CppType* type) {
        il2cpp_functions::Init();

        typeLock.lock();
        if (typeMap.empty()) {
            typeMap[il2cpp_functions::defaults->boolean_class] = "bool";
            typeMap[il2cpp_functions::defaults->byte_class] = "byte";
            typeMap[il2cpp_functions::defaults->sbyte_class] = "sbyte";
            typeMap[il2cpp_functions::defaults->char_class] = "char";
            typeMap[il2cpp_functions::defaults->single_class] = "float";
            typeMap[il2cpp_functions::defaults->double_class] = "double";
            typeMap[il2cpp_functions::defaults->int16_class] = "short";
            typeMap[il2cpp_functions::defaults->uint16_class] = "ushort";
            typeMap[il2cpp_functions::defaults->int32_class] = "int";
            typeMap[il2cpp_functions::defaults->uint32_class] = "uint";
            typeMap[il2cpp_functions::defaults->int64_class] = "long";
            typeMap[il2cpp_functions::defaults->uint64_class] = "ulong";
            typeMap[il2cpp_functions::defaults->object_class] = "object";
            typeMap[il2cpp_functions::defaults->string_class] = "string";
            typeMap[il2cpp_functions::defaults->void_class] = "void";
        }
        auto p = typeMap.find(il2cpp_functions::class_from_il2cpp_type(type));
        if (p != typeMap.end()) {
            typeLock.unlock();
            return p->second;
        } else {
            typeLock.unlock();
            return il2cpp_functions::type_get_name(type);
        }
    }

    bool IsInterface(const Il2CppClass* klass) {
        return (klass->flags & TYPE_ATTRIBUTE_INTERFACE) ||
            (klass->byval_arg.type == IL2CPP_TYPE_VAR) ||
            (klass->byval_arg.type == IL2CPP_TYPE_MVAR);
    }

    std::vector<Il2CppClass*> ClassesFrom(std::vector<Il2CppClass*> classes) { return classes; }
    std::vector<Il2CppClass*> ClassesFrom(std::vector<std::string_view> strings) {
        std::vector<Il2CppClass*> classes;
        for (size_t i = 0; i < strings.size() - 1; i += 2) {
            classes.push_back(GetClassFromName(strings[i].data(), strings[i+1].data()));
        }
        return classes;
    }
    std::vector<const Il2CppType*> TypesFrom(std::vector<const Il2CppType*> types) { return types; }
    std::vector<const Il2CppType*> TypesFrom(std::vector<const Il2CppClass*> classes) { return ClassVecToTypes(classes); }
    std::vector<const Il2CppType*> TypesFrom(std::vector<std::string_view> strings) {
        std::vector<const Il2CppType*> types;
        il2cpp_functions::Init();
        for (size_t i = 0; i < strings.size() - 1; i += 2) {
            auto clazz = GetClassFromName(strings[i].data(), strings[i+1].data());
            types.push_back(il2cpp_functions::class_get_type(clazz));
        }
        return types;
    }

    Il2CppClass* GetParamClass(const MethodInfo* method, int paramIdx) {
        static auto logger = getLogger().WithContext("GetParamClass");
        auto type = RET_0_UNLESS(logger, il2cpp_functions::method_get_param(method, paramIdx));
        return il2cpp_functions::class_from_il2cpp_type(type);
    }

    Il2CppReflectionType* MakeGenericType(Il2CppReflectionType* gt, Il2CppArray* types) {
        static auto logger = getLogger().WithContext("MakeGenericType");
        il2cpp_functions::Init();

        auto runtimeType = RET_0_UNLESS(logger, il2cpp_functions::defaults->runtimetype_class);
        auto makeGenericMethod = RET_0_UNLESS(logger, FindMethodUnsafe(runtimeType, "MakeGenericType", 2));

        Il2CppException* exp = nullptr;
        void* params[] = {reinterpret_cast<void*>(gt), reinterpret_cast<void*>(types)};
        auto genericType = il2cpp_functions::runtime_invoke(makeGenericMethod, nullptr, params, &exp);
        if (exp) {
            logger.error("il2cpp_utils: MakeGenericType: Failed with exception: %s", ExceptionToString(exp).c_str());
            return nullptr;
        }
        return reinterpret_cast<Il2CppReflectionType*>(genericType);
    }

    Il2CppReflectionType* GetSystemType(const Il2CppType* typ) {
        il2cpp_functions::Init();
        return reinterpret_cast<Il2CppReflectionType*>(il2cpp_functions::type_get_object(typ));
    }

    Il2CppReflectionType* GetSystemType(const Il2CppClass* klass) {
        static auto logger = getLogger().WithContext("GetSystemType");
        il2cpp_functions::Init();
        RET_0_UNLESS(logger, klass);

        auto* typ = il2cpp_functions::class_get_type_const(klass);
        return GetSystemType(typ);
    }

    Il2CppReflectionType* GetSystemType(std::string_view nameSpace, std::string_view className) {
        return GetSystemType(il2cpp_utils::GetClassFromName(nameSpace, className));
    }

    void GenericsToStringHelper(Il2CppGenericClass* genClass, std::ostream& os) {
        static auto logger = getLogger().WithContext("GenericsToStringHelper");
        auto genContext = &genClass->context;
        auto* genInst = genContext->class_inst;
        if (!genInst) {
            genInst = genContext->method_inst;
            if (genInst) logger.warning("Missing class_inst! Trying method_inst?");
        }
        if (genInst) {
            os << "<";
            for (size_t i = 0; i < genInst->type_argc; i++) {
                auto typ = genInst->type_argv[i];
                if (i > 0) os << ", ";
                const char* typName = TypeGetSimpleName(typ);
                os << typName;
            }
            os << ">";
        } else {
            logger.warning("context->class_inst missing for genClass!");
        }
    }

    void RemoveDelegate(Il2CppDelegate* delegateInstance, Il2CppDelegate* comparePointer) noexcept {
        auto arrPtr = reinterpret_cast<MulticastDelegate*>(delegateInstance)->delegates;
        std::vector<Il2CppDelegate*> newPtrs(arrPtr->Length());
        for (il2cpp_array_size_t i = 0; i < arrPtr->Length(); i++) {
            if (arrPtr->values[i] != comparePointer) {
                newPtrs.push_back(arrPtr->values[i]);
            }
        }
        reinterpret_cast<MulticastDelegate*>(delegateInstance)->delegates = il2cpp_utils::vectorToArray(newPtrs);
    }

    std::string ClassStandardName(const Il2CppClass* klass, bool generics) {
        il2cpp_functions::Init();
        std::stringstream ss;
        const char* namespaze = il2cpp_functions::class_get_namespace(klass);
        auto* declaring = il2cpp_functions::class_get_declaring_type(klass);
        bool hasNamespace = (namespaze && namespaze[0] != '\0');
        if (!hasNamespace && declaring) {
            ss << ClassStandardName(declaring) << "/";
        } else {
            ss << namespaze << "::";
        }
        ss << il2cpp_functions::class_get_name(klass);

        if (generics) {
            il2cpp_functions::class_is_generic(klass);
            auto* genClass = klass->generic_class;
            if (genClass) {
                GenericsToStringHelper(genClass, ss);
            }
        }
        return ss.str();
    }

    Il2CppObject* createManual(const Il2CppClass* klass) noexcept {
        static auto logger = getLogger().WithContext("createManual");
        if (!klass) {
            logger.error("Cannot create a manual object on a null class!");
            return nullptr;
        }
        if (!klass->initialized) {
            logger.error("Cannot create an object that does not have an initialized class: %p", klass);
            return nullptr;
        }
        auto* obj = reinterpret_cast<Il2CppObject*>(gc_alloc_specific(klass->instance_size));
        if (!obj) {
            logger.error("Failed to allocate GC specific area for instance size: %u", klass->instance_size);
            return nullptr;
        }
        obj->klass = const_cast<Il2CppClass*>(klass);
        // Call cctor, we don't bother making a new thread for the type initializer. BE WARNED!
        if (klass->has_cctor && !klass->cctor_finished_or_no_cctor && !klass->cctor_started) {
            obj->klass->cctor_started = true;
            auto* m = RET_0_UNLESS(logger, FindMethodUnsafe(klass, ".cctor", 0));
            RET_0_UNLESS(logger, il2cpp_utils::RunStaticMethodUnsafe(m));
            obj->klass->cctor_finished_or_no_cctor = true;
        }
        return obj;
    }

    Il2CppObject* createManualThrow(Il2CppClass* const klass) {
        if (!klass->initialized) {
            throw exceptions::StackTraceException(string_format("Cannot create an object that does not have an initialized class: %p", klass));
        }
        auto* obj = reinterpret_cast<Il2CppObject*>(gc_alloc_specific(klass->instance_size));
        if (!obj) {
            throw exceptions::StackTraceException(string_format("Failed to allocate GC specific area for instance size: %u", klass->instance_size));
        }
        obj->klass = const_cast<Il2CppClass*>(klass);
        // Call cctor, we don't bother making a new thread for the type initializer. BE WARNED!
        if (klass->has_cctor && !klass->cctor_finished_or_no_cctor && !klass->cctor_started) {
            obj->klass->cctor_started = true;
            auto* m = FindMethodUnsafe(klass, ".cctor", 0);
            if (!m) {
                throw exceptions::StackTraceException("Failed to find .cctor method!");
            }
            if (!il2cpp_utils::RunStaticMethodUnsafe(m)) {
                throw exceptions::StackTraceException("Failed to run .cctor method!");
            }
            obj->klass->cctor_finished_or_no_cctor = true;
        }
        return obj;
    }

    void* __AllocateUnsafe(std::size_t size) {
        il2cpp_functions::Init();
        // Because we want to allocate this object using C# GC, we will do a bit of a hack here.
        // Essentially, we take advantage of the instance size of System.Object, and then IMMEDIATELY revert it.
        // If we fail for ANY REASON in here, VERY BAD THINGS can happen.
        static auto* objKlass = CRASH_UNLESS(il2cpp_functions::defaults->object_class);
        // Ideally, we make this atomic, but because we aren't using locks anywhere, we hope for the best...
        // TODO: Acquire object class special lock
        auto origSize = objKlass->instance_size;
        objKlass->instance_size = static_cast<decltype(origSize)>(size);
        auto* instance = il2cpp_functions::object_new(objKlass);
        objKlass->instance_size = origSize;
        return instance;
    }

    [[nodiscard]] bool Match(const Il2CppObject* source, const Il2CppClass* klass) noexcept {
        return (source->klass == klass);
    }

    bool AssertMatch(const Il2CppObject* source, Il2CppClass* klass) {
        static auto logger = getLogger().WithContext("AssertMatch");
        il2cpp_functions::Init();
        if (!Match(source, klass)) {
            logger.critical("source with class '%s' does not match class '%s'!",
                ClassStandardName(source->klass).c_str(), ClassStandardName(klass).c_str());
            SAFE_ABORT();
        }
        return true;
    }

    // Contains the map of created MethodInfo* instances
    std::unordered_map<std::pair<Il2CppMethodPointer, bool>, MethodInfo*> delegateMethodInfoMap;

    void ClearDelegates() {
        for (auto itr : delegateMethodInfoMap) {
            free(itr.second);
        }
        delegateMethodInfoMap.clear();
    }

    void ClearDelegate(std::pair<Il2CppMethodPointer, bool> delegate) {
        auto itr = delegateMethodInfoMap.find(delegate);
        if (itr != delegateMethodInfoMap.end()) {
            free(itr->second);
            delegateMethodInfoMap.erase(itr);
        }
    }

    void AddAllocatedDelegate(std::pair<Il2CppMethodPointer, bool> delegate, MethodInfo* mptr) {
        delegateMethodInfoMap.insert({delegate, mptr});
    }
}
