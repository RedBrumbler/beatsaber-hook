#include "../../shared/utils/il2cpp-type-check.hpp"
#include "../../shared/utils/il2cpp-utils.hpp"
#include "../../shared/utils/hashing.hpp"
#include <unordered_map>

namespace il2cpp_utils {
    std::vector<Il2CppClass*> ClassesFrom(std::span<std::string_view> const strings) {
        std::vector<Il2CppClass*> classes;
        classes.reserve((strings.size() - 1) / 2);
        for (size_t i = 0; i < strings.size() - 1; i += 2) {
            classes.push_back(GetClassFromName(strings[i].data(), strings[i + 1].data()));
        }
        return classes;
    }

    std::vector<const Il2CppType*> TypesFrom(std::span<const Il2CppClass*> classes) {
        return ClassVecToTypes(classes);
    }
    std::vector<const Il2CppType*> TypesFrom(std::span<std::string_view> strings) {
        std::vector<const Il2CppType*> types;
        types.reserve((strings.size() - 1) / 2);
        il2cpp_functions::Init();
        for (size_t i = 0; i < strings.size() - 1; i += 2) {
            auto clazz = GetClassFromName(strings[i].data(), strings[i + 1].data());
            types.push_back(il2cpp_functions::class_get_type(clazz));
        }
        return types;
    }

    // It doesn't matter what types these are, they just need to be used correctly within the methods
    static std::unordered_map<std::pair<std::string, std::string>, Il2CppClass*, hash_pair> namesToClassesCache;
    static std::mutex nameHashLock;

    Il2CppClass* FindNested(Il2CppClass* declaring, std::string_view typeName) {
        // logger.info("trying to find: {} ", typeName.data());

        if (!declaring) return nullptr;
        auto token = typeName.find("/");
        bool deeperNested = token != std::string::npos;

        auto subTypeName = deeperNested ? typeName : typeName.substr(0, token);

        void* myIter = nullptr;
        Il2CppClass* found = nullptr;
        while (Il2CppClass* nested = il2cpp_functions::class_get_nested_types(declaring, &myIter)) {
            if (subTypeName == nested->name) {
                found = nested;
                break;
            }
        }

        if (deeperNested) {
            return FindNested(found, typeName.substr(token + 1));
        } else {
            return found;
        }
    }

    Il2CppClass* GetClassFromName(std::string_view name_space, std::string_view type_name) {
        il2cpp_functions::Init();
        auto const& logger = il2cpp_utils::Logger;

        // TODO: avoid creating std::string at any point except new pair insertion via P0919
        // Check cache
        auto key = std::pair<std::string, std::string>(name_space, type_name);
        nameHashLock.lock();
        auto itr = namesToClassesCache.find(key);
        if (itr != namesToClassesCache.end()) {
            nameHashLock.unlock();
            return itr->second;
        }
        nameHashLock.unlock();
        auto dom = RET_0_UNLESS(logger, il2cpp_functions::domain_get());
        size_t assemb_count;
        const Il2CppAssembly** allAssemb = il2cpp_functions::domain_get_assemblies(dom, &assemb_count);

        for (size_t i = 0; i < assemb_count; i++) {
            auto assemb = allAssemb[i];
            auto img = il2cpp_functions::assembly_get_image(assemb);
            if (!img) {
                logger.error("Assembly with name: {} has a null image!", assemb->aname.name);
                continue;
            }
            auto klass = il2cpp_functions::class_from_name(img, name_space.data(), type_name.data());
            if (klass) {
                nameHashLock.lock();
                namesToClassesCache.emplace(key, klass);
                nameHashLock.unlock();
                return klass;
            }
        }

        // we failed to find the class directly, time to check if it is a nested class, and if so look for it
        auto token = type_name.find("/");
        bool nested = token != std::string::npos;
        if (nested) { // this is a nested name
            // get the first part of the nested type_name
            auto declaringTypeName = std::string(type_name.substr(0, token));
            // get the first class, which is the declaring class
            auto declaring = GetClassFromName(name_space, declaringTypeName);
            // recursively look through the nested classes of the declaring class until we run out of tokens ('/') or we run into a problem where we don't find a class
            auto klass = FindNested(declaring, type_name.substr(token + 1));

            if (klass) {
                nameHashLock.lock();
                namesToClassesCache.emplace(key, klass);
                nameHashLock.unlock();
                return klass;
            }
        }

        logger.error("Could not find class with namepace: {} and name: {}",
            name_space.data(), type_name.data());
        return nullptr;
    }

    Il2CppClass* MakeGeneric(const Il2CppClass* klass, std::span<const Il2CppClass* const> const args) {
        il2cpp_functions::Init();
        auto const& logger = il2cpp_utils::Logger;

        static auto typ = RET_0_UNLESS(logger, il2cpp_functions::defaults->systemtype_class);
        auto klassType = RET_0_UNLESS(logger, GetSystemType(klass));

        // Call Type.MakeGenericType on it
        auto arr = il2cpp_functions::array_new_specific(typ, args.size());
        if (!arr) {
            logger.error("[MakeGeneric] Failed to make new array with length: {}", args.size());
            return nullptr;
        }

        int i = 0;
        for (auto arg : args) {
            auto* o = GetSystemType(arg);
            if (!o) {
                logger.error("[MakeGeneric] Failed to get type for {}", il2cpp_functions::class_get_name_const(arg));
                return nullptr;
            }
            il2cpp_array_set(arr, void*, i, reinterpret_cast<void*>(o));
            i++;
        }

        auto* reflection_type = RET_0_UNLESS(logger, MakeGenericType(reinterpret_cast<Il2CppReflectionType*>(klassType), arr));
        auto* ret = RET_0_UNLESS(logger, il2cpp_functions::class_from_system_type(reflection_type));
        return ret;
    }

    Il2CppClass* MakeGeneric(const Il2CppClass* klass, const Il2CppType** types, uint32_t numTypes) {
        il2cpp_functions::Init();
        auto const& logger = il2cpp_utils::Logger;

        static auto typ = RET_0_UNLESS(logger, il2cpp_functions::defaults->systemtype_class);
        auto klassType = RET_0_UNLESS(logger, GetSystemType(klass));

        // Call Type.MakeGenericType on it
        auto arr = il2cpp_functions::array_new_specific(typ, numTypes);
        if (!arr) {
            logger.error("Failed to make new array with length: {}", numTypes);
            return nullptr;
        }

        for (size_t i = 0; i < numTypes; i++) {
            const Il2CppType* arg = types[i];
            auto* o = GetSystemType(arg);
            if (!o) {
                logger.error("Failed to get system type for {}", il2cpp_functions::type_get_name(arg));
                return nullptr;
            }
            il2cpp_array_set(arr, void*, i, reinterpret_cast<void*>(o));
        }

        auto* reflection_type = RET_0_UNLESS(logger, MakeGenericType(reinterpret_cast<Il2CppReflectionType*>(klassType), arr));
        auto* ret = RET_0_UNLESS(logger, il2cpp_functions::class_from_system_type(reflection_type));
        return ret;
    }
}
check_size<sizeof(Il2CppObject), 0x10> il2cppObjectCheck;
check_size<kIl2CppSizeOfArray, 0x20> il2cppArrayCheck;
