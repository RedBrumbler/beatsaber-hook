#ifndef IL2CPP_FUNCTIONS_H
#define IL2CPP_FUNCTIONS_H

#pragma pack(push)

#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <vm/GlobalMetadataFileInternals.h>
#include "logging.hpp"
#include "utils.h"

#if !defined(UNITY_2019) && __has_include("il2cpp-runtime-stats.h")
#define UNITY_2019
#endif

#if defined(__GLIBCXX__) || defined(__GLIBCPP__)
    // We are currently compiling with GNU GCC libstdc++, so we are already using its STL implementation
    typedef std::string gnu_string;
#else

#if !(defined(UNITY_2019) || defined(UNITY_2021))
struct _Rep {
    size_t _M_length;
    size_t _M_capacity;
    int _M_refcount;
};

struct gnu_string {
    const char* _M_p;
    // TODO: why are these needed to prevent a crash on _Type_GetName_ call? They don't contain string data!
    const char padding[9];

    const char* _M_data() const {
        return _M_p;
    }

    const _Rep* _M_rep() const {
        return &((reinterpret_cast<const _Rep *>(_M_data()))[-1]);
    }

    const char* c_str() const {
        return _M_p;
    }

    int length() const {
        return _M_rep()->_M_length;
    }
};
#endif  // UNITY_2019

#endif

struct Il2CppAssembly;
struct Il2CppObject;
struct Il2CppClass;
struct Il2CppImage;
struct Il2CppArray;
struct Il2CppType;
struct MethodInfo;
struct FieldInfo;
struct PropertyInfo;

struct EventInfo;
struct Il2CppDomain;
struct Il2CppReflectionType;
struct Il2CppException;
struct Il2CppProfiler;
struct Il2CppThread;
struct Il2CppReflectionMethod;
struct Il2CppManagedMemorySnapshot;
struct Il2CppStackFrameInfo;
struct Il2CppCustomAttrInfo;
struct Il2CppGenericClass;
struct Il2CppDefaults;

struct Il2CppTypeDefinition;
struct Il2CppGenericParameter;
struct Il2CppGenericContainer;

#include "il2cpp-api-types.h"
#include "il2cpp-metadata.h"
#include "il2cpp-class-internals.h"
#include "utils.h"

typedef std::vector<const Il2CppAssembly*> AssemblyVector;

#ifndef IL2CPP_FUNC_VISIBILITY
#define IL2CPP_FUNC_VISIBILITY private
#endif

#define API_FUNC(rt, name, ...) \
IL2CPP_FUNC_VISIBILITY: \
static rt (*il2cpp_##name)__VA_ARGS__; \
public: \
template<class... TArgs> \
static rt name(TArgs&&... args) { \
    if (!il2cpp_##name) { \
        SAFE_ABORT(); \
    } \
    if constexpr (std::is_same_v<rt, void>) { \
        il2cpp_##name(args...); \
    } else { \
        return il2cpp_##name(args...); \
    } \
}

#define API_FUNC_VISIBLE(rt, name, ...) \
public: \
static rt (*il2cpp_##name)__VA_ARGS__; \
template<class... TArgs> \
static rt name(TArgs&&... args) { \
    if (!il2cpp_##name) { \
        SAFE_ABORT(); \
    } \
    if constexpr (std::is_same_v<rt, void>) { \
        il2cpp_##name(args...); \
    } else { \
        return il2cpp_##name(args...); \
    } \
}
// A class which contains all available il2cpp functions
// Created by zoller27osu
class il2cpp_functions {
  public:
    // These methods autogenerated by Sc2ad:
    #if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC(int, init, (const char* domain_name));
    API_FUNC(int, init_utf16, (const Il2CppChar * domain_name));
    #else
    API_FUNC(void, init, (const char* domain_name));
    API_FUNC(void, init_utf16, (const Il2CppChar * domain_name));
    #endif
    API_FUNC(void, shutdown, ());
    API_FUNC(void, set_config_dir, (const char *config_path));
    API_FUNC(void, set_data_dir, (const char *data_path));
    API_FUNC(void, set_temp_dir, (const char *temp_path));
    API_FUNC(void, set_commandline_arguments, (int argc, const char* const argv[], const char* basedir));
    API_FUNC(void, set_commandline_arguments_utf16, (int argc, const Il2CppChar * const argv[], const char* basedir));
    API_FUNC(void, set_config_utf16, (const Il2CppChar * executablePath));
    API_FUNC(void, set_config, (const char* executablePath));
    API_FUNC(void, set_memory_callbacks, (Il2CppMemoryCallbacks * callbacks));
    API_FUNC(const Il2CppImage*, get_corlib, ());
    API_FUNC(void, add_internal_call, (const char* name, Il2CppMethodPointer method));
    API_FUNC(Il2CppMethodPointer, resolve_icall, (const char* name));
    API_FUNC(void*, alloc, (size_t size));
    API_FUNC(void, free, (void* ptr));
    API_FUNC(Il2CppClass*, array_class_get, (Il2CppClass * element_class, uint32_t rank));
    API_FUNC(uint32_t, array_length, (Il2CppArray * array));
    API_FUNC(uint32_t, array_get_byte_length, (Il2CppArray * array));
    API_FUNC(Il2CppArray*, array_new, (Il2CppClass * elementTypeInfo, il2cpp_array_size_t length));
    API_FUNC(Il2CppArray*, array_new_specific, (Il2CppClass * arrayTypeInfo, il2cpp_array_size_t length));
    API_FUNC(Il2CppArray*, array_new_full, (Il2CppClass * array_class, il2cpp_array_size_t * lengths, il2cpp_array_size_t * lower_bounds));
    API_FUNC(Il2CppClass*, bounded_array_class_get, (Il2CppClass * element_class, uint32_t rank, bool bounded));
    API_FUNC(int, array_element_size, (const Il2CppClass * array_class));
    API_FUNC(const Il2CppImage*, assembly_get_image, (const Il2CppAssembly * assembly));
    #if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC(void, class_for_each, (void(*klassReportFunc)(Il2CppClass* klass, void* userData), void* userData));
    #endif
    API_FUNC(const Il2CppType*, class_enum_basetype, (Il2CppClass * klass));
    API_FUNC(bool, class_is_generic, (const Il2CppClass * klass));
    API_FUNC(bool, class_is_inflated, (const Il2CppClass * klass));
    API_FUNC(bool, class_is_assignable_from, (Il2CppClass * klass, Il2CppClass * oklass));
    API_FUNC(bool, class_is_subclass_of, (Il2CppClass * klass, Il2CppClass * klassc, bool check_interfaces));
    API_FUNC(bool, class_has_parent, (Il2CppClass * klass, Il2CppClass * klassc));
    API_FUNC(Il2CppClass*, class_from_il2cpp_type, (const Il2CppType * type));
    API_FUNC(Il2CppClass*, class_from_name, (const Il2CppImage * image, const char* namespaze, const char *name));
    API_FUNC(Il2CppClass*, class_from_system_type, (Il2CppReflectionType * type));
    API_FUNC(Il2CppClass*, class_get_element_class, (Il2CppClass * klass));
    API_FUNC(const EventInfo*, class_get_events, (Il2CppClass * klass, void* *iter));
    API_FUNC(FieldInfo*, class_get_fields, (Il2CppClass * klass, void* *iter));
    API_FUNC(Il2CppClass*, class_get_nested_types, (Il2CppClass * klass, void* *iter));
    API_FUNC(Il2CppClass*, class_get_interfaces, (Il2CppClass * klass, void* *iter));
    API_FUNC(const PropertyInfo*, class_get_properties, (Il2CppClass * klass, void* *iter));
    API_FUNC(const PropertyInfo*, class_get_property_from_name, (Il2CppClass * klass, const char *name));
    API_FUNC(FieldInfo*, class_get_field_from_name, (Il2CppClass * klass, const char *name));
    API_FUNC(const MethodInfo*, class_get_methods, (Il2CppClass * klass, void* *iter));
    API_FUNC(const MethodInfo*, class_get_method_from_name, (const Il2CppClass * klass, const char* name, int argsCount));
    API_FUNC(const char*, class_get_name, (const Il2CppClass * klass));
    #if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC(void, type_get_name_chunked, (const Il2CppType * type, void(*chunkReportFunc)(void* data, void* userData), void* userData));
    #endif
    API_FUNC(const char*, class_get_namespace, (const Il2CppClass * klass));
    API_FUNC(Il2CppClass*, class_get_parent, (Il2CppClass * klass));
    API_FUNC(Il2CppClass*, class_get_declaring_type, (const Il2CppClass * klass));
    API_FUNC(int32_t, class_instance_size, (Il2CppClass * klass));
    API_FUNC(size_t, class_num_fields, (const Il2CppClass * enumKlass));
    API_FUNC(bool, class_is_valuetype, (const Il2CppClass * klass));
    API_FUNC(int32_t, class_value_size, (Il2CppClass * klass, uint32_t * align));
    API_FUNC(bool, class_is_blittable, (const Il2CppClass * klass));
    API_FUNC(int, class_get_flags, (const Il2CppClass * klass));
    API_FUNC(bool, class_is_abstract, (const Il2CppClass * klass));
    API_FUNC(bool, class_is_interface, (const Il2CppClass * klass));
    API_FUNC(int, class_array_element_size, (const Il2CppClass * klass));
    API_FUNC(Il2CppClass*, class_from_type, (const Il2CppType * type));
    API_FUNC(const Il2CppType*, class_get_type, (Il2CppClass * klass));
    API_FUNC(uint32_t, class_get_type_token, (Il2CppClass * klass));
    API_FUNC(bool, class_has_attribute, (Il2CppClass * klass, Il2CppClass * attr_class));
    API_FUNC(bool, class_has_references, (Il2CppClass * klass));
    API_FUNC(bool, class_is_enum, (const Il2CppClass * klass));
    API_FUNC(const Il2CppImage*, class_get_image, (Il2CppClass * klass));
    API_FUNC(const char*, class_get_assemblyname, (const Il2CppClass * klass));
    API_FUNC(int, class_get_rank, (const Il2CppClass * klass));
    #if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC(uint32_t, class_get_data_size, (const Il2CppClass * klass));
    API_FUNC(void*, class_get_static_field_data, (const Il2CppClass * klass));
    #endif
    API_FUNC(size_t, class_get_bitmap_size, (const Il2CppClass * klass));
    API_FUNC(void, class_get_bitmap, (Il2CppClass * klass, size_t * bitmap));
    API_FUNC(bool, stats_dump_to_file, (const char *path));
    API_FUNC(uint64_t, stats_get_value, (Il2CppStat stat));
    API_FUNC(Il2CppDomain*, domain_get, ());
    API_FUNC(const Il2CppAssembly*, domain_assembly_open, (Il2CppDomain * domain, const char* name));
    API_FUNC(const Il2CppAssembly**, domain_get_assemblies, (const Il2CppDomain * domain, size_t * size));
    #if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC(void, raise_exception, (Il2CppException*));
    #endif
    API_FUNC(Il2CppException*, exception_from_name_msg, (const Il2CppImage * image, const char *name_space, const char *name, const char *msg));
    API_FUNC(Il2CppException*, get_exception_argument_null, (const char *arg));
    API_FUNC(void, format_exception, (const Il2CppException * ex, char* message, int message_size));
    API_FUNC(void, format_stack_trace, (const Il2CppException * ex, char* output, int output_size));
    API_FUNC(void, unhandled_exception, (Il2CppException*));
    API_FUNC(int, field_get_flags, (FieldInfo * field));
    API_FUNC(const char*, field_get_name, (FieldInfo * field));
    API_FUNC(Il2CppClass*, field_get_parent, (FieldInfo * field));
    API_FUNC(size_t, field_get_offset, (FieldInfo * field));
    API_FUNC(const Il2CppType*, field_get_type, (FieldInfo * field));
    API_FUNC(void, field_get_value, (Il2CppObject * obj, FieldInfo * field, void *value));
    API_FUNC(Il2CppObject*, field_get_value_object, (FieldInfo * field, Il2CppObject * obj));
    API_FUNC(bool, field_has_attribute, (FieldInfo * field, Il2CppClass * attr_class));
    API_FUNC(void, field_set_value, (Il2CppObject * obj, FieldInfo * field, void *value));
    API_FUNC(void, field_static_get_value, (FieldInfo * field, void *value));
    API_FUNC(void, field_static_set_value, (FieldInfo * field, void *value));
    API_FUNC(void, field_set_value_object, (Il2CppObject * instance, FieldInfo * field, Il2CppObject * value));
    #if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC(bool, field_is_literal, (FieldInfo * field));
    #endif
    API_FUNC(void, gc_collect, (int maxGenerations));
    API_FUNC(int32_t, gc_collect_a_little, ());
    API_FUNC(void, gc_disable, ());
    API_FUNC(void, gc_enable, ());
    API_FUNC(bool, gc_is_disabled, ());
    #if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC(int64_t, gc_get_max_time_slice_ns, ());
    API_FUNC(void, gc_set_max_time_slice_ns, (int64_t maxTimeSlice));
    API_FUNC(bool, gc_is_incremental, ());
    #endif
    API_FUNC(int64_t, gc_get_used_size, ());
    API_FUNC(int64_t, gc_get_heap_size, ());
    API_FUNC(void, gc_wbarrier_set_field, (Il2CppObject * obj, void **targetAddress, void *object));
    #if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC(bool, gc_has_strict_wbarriers, ());
    API_FUNC(void, gc_set_external_allocation_tracker, (void(*func)(void*, size_t, int)));
    API_FUNC(void, gc_set_external_wbarrier_tracker, (void(*func)(void**)));
    API_FUNC(void, gc_foreach_heap, (void(*func)(void* data, void* userData), void* userData));
    API_FUNC(void, stop_gc_world, ());
    API_FUNC(void, start_gc_world, ());
    #endif
    API_FUNC(uint32_t, gchandle_new, (Il2CppObject * obj, bool pinned));
    API_FUNC(uint32_t, gchandle_new_weakref, (Il2CppObject * obj, bool track_resurrection));
    API_FUNC(Il2CppObject*, gchandle_get_target, (uint32_t gchandle));
    API_FUNC(void, gchandle_free, (uint32_t gchandle));
    #if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC(void, gchandle_foreach_get_target, (void(*func)(void* data, void* userData), void* userData));
    API_FUNC(uint32_t, object_header_size, ());
    API_FUNC(uint32_t, array_object_header_size, ());
    API_FUNC(uint32_t, offset_of_array_length_in_array_object_header, ());
    API_FUNC(uint32_t, offset_of_array_bounds_in_array_object_header, ());
    API_FUNC(uint32_t, allocation_granularity, ());
    #endif
    #ifndef UNITY_2021
    API_FUNC(void*, unity_liveness_calculation_begin, (Il2CppClass * filter, int max_object_count, il2cpp_register_object_callback callback, void* userdata, il2cpp_WorldChangedCallback onWorldStarted, il2cpp_WorldChangedCallback onWorldStopped));
    API_FUNC(void, unity_liveness_calculation_end, (void* state));
    #else
    API_FUNC(void*, il2cpp_unity_liveness_allocate_struct, (Il2CppClass * filter, int max_object_count, il2cpp_register_object_callback callback, void* userdata, il2cpp_liveness_reallocate_callback reallocate));
    API_FUNC(void, il2cpp_unity_liveness_finalize, (void* state));
    API_FUNC(void, il2cpp_unity_liveness_free_struct, (void* state));
    #endif
    API_FUNC(void, unity_liveness_calculation_from_root, (Il2CppObject * root, void* state));
    API_FUNC(void, unity_liveness_calculation_from_statics, (void* state));
    API_FUNC(const Il2CppType*, method_get_return_type, (const MethodInfo * method));
    API_FUNC(Il2CppClass*, method_get_declaring_type, (const MethodInfo * method));
    API_FUNC(const char*, method_get_name, (const MethodInfo * method));
    API_FUNC(const MethodInfo*, method_get_from_reflection, (const Il2CppReflectionMethod * method));
    API_FUNC(Il2CppReflectionMethod*, method_get_object, (const MethodInfo * method, Il2CppClass * refclass));
    API_FUNC(bool, method_is_generic, (const MethodInfo * method));
    API_FUNC(bool, method_is_inflated, (const MethodInfo * method));
    API_FUNC(bool, method_is_instance, (const MethodInfo * method));
    API_FUNC(uint32_t, method_get_param_count, (const MethodInfo * method));
    API_FUNC(const Il2CppType*, method_get_param, (const MethodInfo * method, uint32_t index));
    API_FUNC(Il2CppClass*, method_get_class, (const MethodInfo * method));
    API_FUNC(bool, method_has_attribute, (const MethodInfo * method, Il2CppClass * attr_class));
    API_FUNC(uint32_t, method_get_flags, (const MethodInfo * method, uint32_t * iflags));
    API_FUNC(uint32_t, method_get_token, (const MethodInfo * method));
    API_FUNC(const char*, method_get_param_name, (const MethodInfo * method, uint32_t index));

    // ONLY IF THE PROFILER EXISTS FOR UNITY_2019
    API_FUNC(void, profiler_install, (Il2CppProfiler * prof, Il2CppProfileFunc shutdown_callback));
    API_FUNC(void, profiler_set_events, (Il2CppProfileFlags events));
    API_FUNC(void, profiler_install_enter_leave, (Il2CppProfileMethodFunc enter, Il2CppProfileMethodFunc fleave));
    API_FUNC(void, profiler_install_allocation, (Il2CppProfileAllocFunc callback));
    API_FUNC(void, profiler_install_gc, (Il2CppProfileGCFunc callback, Il2CppProfileGCResizeFunc heap_resize_callback));
    API_FUNC(void, profiler_install_fileio, (Il2CppProfileFileIOFunc callback));
    API_FUNC(void, profiler_install_thread, (Il2CppProfileThreadFunc start, Il2CppProfileThreadFunc end));

    API_FUNC(uint32_t, property_get_flags, (const PropertyInfo * prop));
    API_FUNC(const MethodInfo*, property_get_get_method, (const PropertyInfo * prop));
    API_FUNC(const MethodInfo*, property_get_set_method, (const PropertyInfo * prop));
    API_FUNC(const char*, property_get_name, (const PropertyInfo * prop));
    API_FUNC(Il2CppClass*, property_get_parent, (const PropertyInfo * prop));
    API_FUNC(Il2CppClass*, object_get_class, (Il2CppObject * obj));
    API_FUNC(uint32_t, object_get_size, (Il2CppObject * obj));
    API_FUNC(const MethodInfo*, object_get_virtual_method, (Il2CppObject * obj, const MethodInfo * method));
    API_FUNC(Il2CppObject*, object_new, (const Il2CppClass * klass));
    // Always returns (void*, (obj + 1)
    API_FUNC(void*, object_unbox, (Il2CppObject * obj));
    // If klass is not a ValueType, returns (Il2CppObject*, (*data), else boxes
    API_FUNC(Il2CppObject*, value_box, (Il2CppClass * klass, void* data));
    API_FUNC(void, monitor_enter, (Il2CppObject * obj));
    API_FUNC(bool, monitor_try_enter, (Il2CppObject * obj, uint32_t timeout));
    API_FUNC(void, monitor_exit, (Il2CppObject * obj));
    API_FUNC(void, monitor_pulse, (Il2CppObject * obj));
    API_FUNC(void, monitor_pulse_all, (Il2CppObject * obj));
    API_FUNC(void, monitor_wait, (Il2CppObject * obj));
    API_FUNC(bool, monitor_try_wait, (Il2CppObject * obj, uint32_t timeout));
    API_FUNC(Il2CppObject*, runtime_invoke, (const MethodInfo * method, void *obj, void **params, Il2CppException **exc));
    API_FUNC(Il2CppObject*, runtime_invoke_convert_args, (const MethodInfo * method, void *obj, Il2CppObject **params, int paramCount, Il2CppException **exc));
    API_FUNC(void, runtime_class_init, (Il2CppClass * klass));
    API_FUNC(void, runtime_object_init, (Il2CppObject * obj));
    API_FUNC(void, runtime_object_init_exception, (Il2CppObject * obj, Il2CppException** exc));
    API_FUNC(void, runtime_unhandled_exception_policy_set, (Il2CppRuntimeUnhandledExceptionPolicy value));
    API_FUNC(int32_t, string_length, (Il2CppString * str));
    API_FUNC(Il2CppChar*, string_chars, (Il2CppString * str));
    API_FUNC(Il2CppString*, string_new, (const char* str));
    API_FUNC(Il2CppString*, string_new_len, (const char* str, uint32_t length));
    API_FUNC(Il2CppString*, string_new_utf16, (const Il2CppChar * text, int32_t len));
    API_FUNC(Il2CppString*, string_new_wrapper, (const char* str));
    API_FUNC(Il2CppString*, string_intern, (Il2CppString * str));
    API_FUNC(Il2CppString*, string_is_interned, (Il2CppString * str));
    API_FUNC(Il2CppThread*, thread_current, ());
    API_FUNC(Il2CppThread*, thread_attach, (Il2CppDomain * domain));
    API_FUNC(void, thread_detach, (Il2CppThread * thread));
    API_FUNC(Il2CppThread**, thread_get_all_attached_threads, (size_t * size));
    API_FUNC(bool, is_vm_thread, (Il2CppThread * thread));
    API_FUNC(void, current_thread_walk_frame_stack, (Il2CppFrameWalkFunc func, void* user_data));
    API_FUNC(void, thread_walk_frame_stack, (Il2CppThread * thread, Il2CppFrameWalkFunc func, void* user_data));
    API_FUNC(bool, current_thread_get_top_frame, (Il2CppStackFrameInfo * frame));
    API_FUNC(bool, thread_get_top_frame, (Il2CppThread * thread, Il2CppStackFrameInfo * frame));
    API_FUNC(bool, current_thread_get_frame_at, (int32_t offset, Il2CppStackFrameInfo * frame));
    API_FUNC(bool, thread_get_frame_at, (Il2CppThread * thread, int32_t offset, Il2CppStackFrameInfo * frame));
    API_FUNC(int32_t, current_thread_get_stack_depth, ());
    API_FUNC(int32_t, thread_get_stack_depth, (Il2CppThread * thread));
    #if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC(void, override_stack_backtrace, (Il2CppBacktraceFunc stackBacktraceFunc));
    #endif
    API_FUNC(Il2CppObject*, type_get_object, (const Il2CppType * type));
    API_FUNC(int, type_get_type, (const Il2CppType * type));
    API_FUNC(Il2CppClass*, type_get_class_or_element_class, (const Il2CppType * type));
    API_FUNC(char*, type_get_name, (const Il2CppType * type));
    API_FUNC(bool, type_is_byref, (const Il2CppType * type));
    API_FUNC(uint32_t, type_get_attrs, (const Il2CppType * type));
    API_FUNC(bool, type_equals, (const Il2CppType * type, const Il2CppType * otherType));
    API_FUNC(char*, type_get_assembly_qualified_name, (const Il2CppType * type));
    #if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC(bool, type_is_static, (const Il2CppType * type));
    API_FUNC(bool, type_is_pointer_type, (const Il2CppType * type));
    #endif
    API_FUNC(const Il2CppAssembly*, image_get_assembly, (const Il2CppImage * image));
    API_FUNC(const char*, image_get_name, (const Il2CppImage * image));
    API_FUNC(const char*, image_get_filename, (const Il2CppImage * image));
    API_FUNC(const MethodInfo*, image_get_entry_point, (const Il2CppImage * image));
    API_FUNC(size_t, image_get_class_count, (const Il2CppImage * image));
    API_FUNC(const Il2CppClass*, image_get_class, (const Il2CppImage * image, size_t index));
    API_FUNC(Il2CppManagedMemorySnapshot*, capture_memory_snapshot, ());
    API_FUNC(void, free_captured_memory_snapshot, (Il2CppManagedMemorySnapshot * snapshot));
    API_FUNC(void, set_find_plugin_callback, (Il2CppSetFindPlugInCallback method));
    API_FUNC(void, register_log_callback, (Il2CppLogCallback method));
    API_FUNC(void, debugger_set_agent_options, (const char* options));
    API_FUNC(bool, is_debugger_attached, ());
    #if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC(void, register_debugger_agent_transport, (Il2CppDebuggerTransport * debuggerTransport));
    API_FUNC(bool, debug_get_method_info, (const MethodInfo*, Il2CppMethodDebugInfo * methodDebugInfo));
    #endif
    API_FUNC(void, unity_install_unitytls_interface, (const void* unitytlsInterfaceStruct));
    API_FUNC(Il2CppCustomAttrInfo*, custom_attrs_from_class, (Il2CppClass * klass));
    API_FUNC(Il2CppCustomAttrInfo*, custom_attrs_from_method, (const MethodInfo * method));
    API_FUNC(Il2CppObject*, custom_attrs_get_attr, (Il2CppCustomAttrInfo * ainfo, Il2CppClass * attr_klass));
    API_FUNC(bool, custom_attrs_has_attr, (Il2CppCustomAttrInfo * ainfo, Il2CppClass * attr_klass));
    API_FUNC(Il2CppArray*, custom_attrs_construct, (Il2CppCustomAttrInfo * cinfo));
    API_FUNC(void, custom_attrs_free, (Il2CppCustomAttrInfo * ainfo));
    #if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC(void, class_set_userdata, (Il2CppClass * klass, void* userdata));
    API_FUNC(int, class_get_userdata_offset, ());
    #endif

    // MANUALLY DEFINED CONST DEFINITIONS
    API_FUNC(const Il2CppType*, class_get_type_const, (const Il2CppClass * klass));
    API_FUNC(const char*, class_get_name_const, (const Il2CppClass * klass));

    // SELECT NON-API LIBIL2CPP FUNCTIONS:
    API_FUNC_VISIBLE(bool, Class_Init, (Il2CppClass* klass));

    API_FUNC_VISIBLE(Il2CppClass*, MetadataCache_GetTypeInfoFromHandle, (Il2CppMetadataTypeHandle index));
    API_FUNC_VISIBLE(Il2CppClass*, MetadataCache_GetTypeInfoFromTypeIndex, (TypeIndex index));

    API_FUNC_VISIBLE(Il2CppClass*, GlobalMetadata_GetTypeInfoFromTypeDefinitionIndex, (TypeDefinitionIndex index));
    API_FUNC_VISIBLE(Il2CppClass*, GlobalMetadata_GetTypeInfoFromHandle, (TypeDefinitionIndex index));
#if defined(UNITY_2019) || defined(UNITY_2021)
    API_FUNC_VISIBLE(std::string, _Type_GetName_, (const Il2CppType *type, Il2CppTypeNameFormat format));
#else
    API_FUNC_VISIBLE(gnu_string, _Type_GetName_, (const Il2CppType *type, Il2CppTypeNameFormat format));
#endif
    API_FUNC_VISIBLE(void, GC_free, (void* addr));

    API_FUNC_VISIBLE(void, GarbageCollector_SetWriteBarrier, (void** ptr));
    API_FUNC_VISIBLE(void*, GarbageCollector_AllocateFixed, (size_t sz, void* descr));

    API_FUNC_VISIBLE(Il2CppClass*, Class_FromIl2CppType, (Il2CppType* typ));
    API_FUNC_VISIBLE(Il2CppClass*, Class_GetPtrClass, (Il2CppClass* elementClass));
    API_FUNC_VISIBLE(Il2CppClass*, GenericClass_GetClass, (Il2CppGenericClass* gclass));
    API_FUNC_VISIBLE(AssemblyVector*, Assembly_GetAllAssemblies, ());

    private:
    static bool find_GC_free(const uint32_t* Runtime_Shutdown);
    static bool find_GC_SetWriteBarrier(const uint32_t* set_wbarrier_field);
    static bool trace_GC_AllocFixed(const uint32_t* DomainGetCurrent);
    static bool find_GC_AllocFixed(const uint32_t* DomainGetCurrent);

    static const Il2CppMetadataRegistration** s_Il2CppMetadataRegistrationPtr;
    static const void** s_GlobalMetadataPtr;
    static const Il2CppGlobalMetadataHeader** s_GlobalMetadataHeaderPtr;

    public:
    static bool hasGCFuncs;
    // You must il2cpp_functions::free the char* when you are done with it
    static char* Type_GetName(const Il2CppType *type, Il2CppTypeNameFormat format);
    static std::remove_pointer_t<decltype(s_GlobalMetadataPtr)> s_GlobalMetadata;
    static std::remove_pointer_t<decltype(s_GlobalMetadataHeaderPtr)> s_GlobalMetadataHeader;
    static std::remove_pointer_t<decltype(s_Il2CppMetadataRegistrationPtr)> s_Il2CppMetadataRegistration;

    static const Il2CppDefaults* defaults;

    // must be done on-demand because the pointers aren't necessarily correct at the time of il2cpp_functions::Init
    static void CheckS_GlobalMetadata() {
        if (!s_GlobalMetadataHeader) {
            static auto& logger = getFuncLogger();
            s_GlobalMetadata = *CRASH_UNLESS(il2cpp_functions::s_GlobalMetadataPtr);
            s_GlobalMetadataHeader = *CRASH_UNLESS(il2cpp_functions::s_GlobalMetadataHeaderPtr);
            s_Il2CppMetadataRegistration = *CRASH_UNLESS(il2cpp_functions::s_Il2CppMetadataRegistrationPtr);
            logger.debug("sanity: %X (should be 0xFAB11BAF)", s_GlobalMetadataHeader->sanity);
            logger.debug("version: %i", s_GlobalMetadataHeader->version);
            CRASH_UNLESS((uint32_t)s_GlobalMetadataHeader->sanity == 0xFAB11BAF);
            logger.debug("typeDefinitionsOffset: %i", s_GlobalMetadataHeader->typeDefinitionsOffset);
            logger.debug("exportedTypeDefinitionsOffset: %i", s_GlobalMetadataHeader->exportedTypeDefinitionsOffset);
            logger.debug("nestedTypesOffset: %i", s_GlobalMetadataHeader->nestedTypesOffset);
            // TODO: use il2cpp_functions::defaults to define the il2cpp_defaults variable mentioned in il2cpp-class-internals.h
        }
    }

    // COPIES OF FREQUENTLY INLINED NON-API LIBIL2CPP FUNCTIONS:
    static const char* MetadataCache_GetStringFromIndex(StringIndex index);
    static const Il2CppTypeDefinition* MetadataCache_GetTypeDefinitionFromIndex(TypeDefinitionIndex index);
    static TypeDefinitionIndex MetadataCache_GetExportedTypeFromIndex(TypeDefinitionIndex index);
    static const Il2CppGenericContainer* MetadataCache_GetGenericContainerFromIndex(GenericContainerIndex index);
    static const Il2CppGenericParameter* MetadataCache_GetGenericParameterFromIndex(GenericParameterIndex index);

    // static Il2CppMetadataGenericParameterHandle GetGenericParameterFromIndexInternal(GenericParameterIndex index)
    // static GenericParameterIndex GetIndexForGenericParameter(Il2CppMetadataGenericParameterHandle handle)

    static GenericParameterIndex MetadataCache_GetGenericParameterIndexFromParameter(Il2CppMetadataGenericParameterHandle handle) {
        const Il2CppGenericParameter* genericParameter = reinterpret_cast<const Il2CppGenericParameter*>(handle);
        const Il2CppGenericParameter* genericParameters = (const Il2CppGenericParameter*)((const char*)s_GlobalMetadata + s_GlobalMetadataHeader->genericParametersOffset);

        IL2CPP_ASSERT(genericParameter >= genericParameters && genericParameter < genericParameters + s_GlobalMetadataHeader->genericParametersSize / sizeof(Il2CppGenericParameter));

        ptrdiff_t index = reinterpret_cast<Il2CppGenericParameter const*>(genericParameter) - reinterpret_cast<Il2CppGenericParameter const*>(genericParameters);
        IL2CPP_ASSERT(index <= std::numeric_limits<GenericParameterIndex>::max());
        return static_cast<GenericParameterIndex>(index);
    }

    static const Il2CppTypeDefinition* MetadataCache_GetTypeDefinition(Il2CppClass* klass) {
        return reinterpret_cast<const Il2CppTypeDefinition*>(klass->typeMetadataHandle);
    }

    static GenericParameterIndex MetadataCache_GetGenericContainerIndex(Il2CppClass* klass) {
        return MetadataCache_GetTypeDefinition(klass)->genericContainerIndex;
    }
    static Il2CppClass* MetadataCache_GetNestedTypeFromIndex(NestedTypeIndex index);
    static TypeDefinitionIndex MetadataCache_GetIndexForTypeDefinition(const Il2CppClass* typeDefinition);

    // Whether all of the il2cpp functions have been initialized or not
    static bool initialized;
    // Initializes all of the IL2CPP functions via dlopen and dlsym for use.
    static void Init();

    static LoggerContextObject& getFuncLogger();
};

#undef API_FUNC

#pragma pack(pop)

#endif /* IL2CPP_FUNCTIONS_H */