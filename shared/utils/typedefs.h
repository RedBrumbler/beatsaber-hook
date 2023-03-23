#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#pragma pack(push)

#include <stdio.h>
#include <stdlib.h>
#include <type_traits>
#include <initializer_list>

#include <cassert>
// For including il2cpp properly
#ifdef _MSC_VER
#undef _MSC_VER
#endif
#ifndef __GNUC__
#define __GNUC__
#endif

#define NET_4_0 true
#include "il2cpp-config.h"
#include "il2cpp-api-types.h"
#include "il2cpp-class-internals.h"
#include "il2cpp-tabledefs.h"

#ifdef __cplusplus

template<class T>
struct Array;

extern "C" {
#endif

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#if __has_include("System/Array.hpp") && !defined(NO_CODEGEN_USE)
#define HAS_CODEGEN
#include <stdint.h>
#include <stddef.h>
#include "il2cpp-windowsruntime-types.h"
#else
// TODO: find a way to include this without putting the types in the global namespace?
#include "manual-il2cpp-typedefs.h"
#endif

#include "il2cpp-functions.hpp"
#include "il2cpp-utils-methods.hpp"
#include "il2cpp-type-check.hpp"

#include "typedefs-array.hpp"
#include "typedefs-delegate.hpp"
#include "typedefs-wrappers.hpp"

#include <stdint.h>

namespace il2cpp_utils {
    namespace array_utils {
        static char* il2cpp_array_addr_with_size(Il2CppArray *array, int32_t size, uintptr_t idx)
        {
            return ((char*)array) + kIl2CppSizeOfArray + size * idx;
        }
        #define load_array_elema(arr, idx, size) ((((uint8_t*)(arr)) + kIl2CppSizeOfArray) + ((size) * (idx)))

        #define il2cpp_array_setwithsize(array, elementSize, index, value)  \
            do {    \
                void*__p = (void*) il2cpp_utils::array_utils::il2cpp_array_addr_with_size ((array), elementSize, (index)); \
                memcpy(__p, &(value), elementSize); \
            } while (0)
        #define il2cpp_array_setrefwithsize(array, elementSize, index, value)  \
            do {    \
                void*__p = (void*) il2cpp_utils::array_utils::il2cpp_array_addr_with_size ((array), elementSize, (index)); \
                memcpy(__p, value, elementSize); \
                } while (0)
        #define il2cpp_array_addr(array, type, index) ((type*)(void*) il2cpp_utils::array_utils::il2cpp_array_addr_with_size (array, sizeof (type), index))
        #define il2cpp_array_get(array, type, index) ( *(type*)il2cpp_array_addr ((array), type, (index)) )
        #define il2cpp_array_set(array, type, index, value)    \
            do {    \
                type *__p = (type *) il2cpp_array_addr ((array), type, (index));    \
                *__p = (value); \
            } while (0)
        #define il2cpp_array_setref(array, index, value)  \
            do {    \
                void* *__p = (void* *) il2cpp_array_addr ((array), void*, (index)); \
                /* il2cpp_gc_wbarrier_set_arrayref ((array), __p, (MonoObject*)(value));    */\
                *__p = (value);    \
            } while (0)
    }
}

#ifdef HAS_CODEGEN

#ifdef USE_CODEGEN_FIELDS
#define _HAD_CODEGEN_FIELDS
#else
#define USE_CODEGEN_FIELDS
#endif

#include "System/String.hpp"
struct Il2CppString : public System::String {};
#endif

#ifdef HAS_CODEGEN
#include "System/Collections/Generic/List_1.hpp"
template<class T>
using List = System::Collections::Generic::List_1<T>;
#else
// System.Collections.Generic.List
template<class T>
struct List : Il2CppObject
{
    Array<T>* items;
    int size;
    int version;
    Il2CppObject* syncRoot;
};
DEFINE_IL2CPP_ARG_TYPE_GENERIC_CLASS(List, "System.Collections.Generic", "List`1");
#endif
#include "typedefs-list.hpp"

#ifdef HAS_CODEGEN
// TODO: QiCache and Il2CppComObject ("System.__Il2CppComObject (dummy type that replaces System.__ComObject)")

#include "System/AppDomain.hpp"
// self-typedef'd in il2cpp-class-internals.h
struct Il2CppAppDomain : public System::AppDomain {};
NEED_NO_BOX(Il2CppAppDomain);

#include "System/AppDomainSetup.hpp"
// self-typedef'd in il2cpp-class-internals.h
struct Il2CppAppDomainSetup : public System::AppDomainSetup {};
NEED_NO_BOX(Il2CppAppDomainSetup);

#include "System/ArgumentException.hpp"
typedef System::ArgumentException Il2CppArgumentException;

// TODO: Il2CppDecimal is System::Decimal?

typedef enum Il2CppDecimalCompareResult
{
    IL2CPP_DECIMAL_CMP_LT = -1,
    IL2CPP_DECIMAL_CMP_EQ,
    IL2CPP_DECIMAL_CMP_GT
} Il2CppDecimalCompareResult;

// TODO: Il2CppDouble, Il2CppDouble_double are System::Double?

#include "System/Exception.hpp"
// self-typedef'd in il2cpp-api-types.h
struct Il2CppException : public System::Exception {};

#include "System/IOAsyncResult.hpp"
typedef System::IOAsyncResult Il2CppIOAsyncResult;

#include "System/IOSelectorJob.hpp"
typedef System::IOSelectorJob Il2CppIOSelectorJob;

#include "System/MarshalByRefObject.hpp"
typedef System::MarshalByRefObject Il2CppMarshalByRefObject;

#include "System/MonoAsyncCall.hpp"
typedef System::MonoAsyncCall Il2CppAsyncCall;

#include "System/MonoType.hpp"
struct Il2CppReflectionMonoType : public System::MonoType {
    const Il2CppType* GetIl2CppType() const {
        return reinterpret_cast<Il2CppType*>(impl.value.m_value);
    }
};

#include "System/RuntimeType.hpp"
struct Il2CppReflectionRuntimeType : public System::RuntimeType {};

// TODO: Il2CppSingle, Il2CppSingle_float are System::Single?

#include "System/SystemException.hpp"
typedef System::SystemException Il2CppSystemException;

#include "System/Type.hpp"
// self-typedef'd in il2cpp-api-types.h
struct Il2CppReflectionType : public System::Type {};

#include "System/TypedReference.hpp"
typedef System::TypedReference Il2CppTypedRef;

#include "System/Diagnostics/StackFrame.hpp"
typedef System::Diagnostics::StackFrame Il2CppStackFrame;

// TODO: Il2CppCalendarData is System::Globalization::CalendarData minus 4 fields at the end?

// TODO: Il2CppCultureData is System::Globalization::CultureData minus 13 fields at the end?

#include "System/Globalization/CultureInfo.hpp"
typedef System::Globalization::CultureInfo Il2CppCultureInfo;

#include "System/Globalization/DateTimeFormatInfo.hpp"
typedef System::Globalization::DateTimeFormatInfo Il2CppDateTimeFormatInfo;

#include "System/Globalization/NumberFormatInfo.hpp"
typedef System::Globalization::NumberFormatInfo Il2CppNumberFormatInfo;

#include "System/Globalization/RegionInfo.hpp"
typedef System::Globalization::RegionInfo Il2CppRegionInfo;

#include "System/Globalization/SortKey.hpp"
typedef System::Globalization::SortKey Il2CppSortKey;

#include "System/Net/SocketAddress.hpp"
typedef System::Net::SocketAddress Il2CppSocketAddress;

// "Corresponds to Mono's internal System.Net.Sockets.Socket.SocketAsyncResult class. Has no relation to Il2CppAsyncResult."
#include "System/Net/Sockets/SocketAsyncResult.hpp"
typedef System::Net::Sockets::SocketAsyncResult Il2CppSocketAsyncResult;

#include "System/Reflection/EventInfo.hpp"
typedef System::Reflection::EventInfo Il2CppReflectionEvent;

#include "System/Reflection/MonoEvent.hpp"
typedef System::Reflection::MonoEvent Il2CppReflectionMonoEvent;

#include "System/Reflection/MonoEventInfo.hpp"
typedef System::Reflection::MonoEventInfo Il2CppReflectionMonoEventInfo;

#include "System/Reflection/MonoField.hpp"
typedef System::Reflection::MonoField Il2CppReflectionField;

#include "System/Reflection/MonoProperty.hpp"
typedef System::Reflection::MonoProperty Il2CppReflectionProperty;

#include "System/Reflection/MonoMethod.hpp"
// self-typedef'd in il2cpp-api-types.h
struct Il2CppReflectionMethod : public System::Reflection::MonoMethod {};

#if __has_include("System/Reflection/MonoGenericMethod.hpp")
#include "System/Reflection/MonoGenericMethod.hpp"
typedef System::Reflection::MonoGenericMethod Il2CppReflectionGenericMethod;
#endif

#include "System/Reflection/MonoMethodInfo.hpp"
typedef System::Reflection::MonoMethodInfo Il2CppMethodInfo;

#include "System/Reflection/MonoPropertyInfo.hpp"
typedef System::Reflection::MonoPropertyInfo Il2CppPropertyInfo;

#include "System/Reflection/ParameterInfo.hpp"
typedef System::Reflection::ParameterInfo Il2CppReflectionParameter;

#include "System/Reflection/Module.hpp"
typedef System::Reflection::Module Il2CppReflectionModule;

#include "System/Reflection/AssemblyName.hpp"
typedef System::Reflection::AssemblyName Il2CppReflectionAssemblyName;

#include "System/Reflection/Assembly.hpp"
typedef System::Reflection::Assembly Il2CppReflectionAssembly;

#include "System/Reflection/Emit/UnmanagedMarshal.hpp"
typedef System::Reflection::Emit::UnmanagedMarshal Il2CppReflectionMarshal;

/* Stripped in 1.13.5 Update
#include "System/Reflection/ManifestResourceInfo.hpp"
typedef System::Reflection::ManifestResourceInfo Il2CppManifestResourceInfo;
*/

#include "System/Reflection/Pointer.hpp"
typedef System::Reflection::Pointer Il2CppReflectionPointer;

// TODO: Il2CppResourceLocation seems to be the System.Reflection.ResourceLocation enum

#include "System/Runtime/InteropServices/ErrorWrapper.hpp"
typedef System::Runtime::InteropServices::ErrorWrapper Il2CppErrorWrapper;

// "Inherited by Microsoft.Win32.SafeHandles.SafeWaitHandle"
#include "System/Runtime/InteropServices/SafeHandle.hpp"
typedef System::Runtime::InteropServices::SafeHandle Il2CppSafeHandle;

#include "System/Runtime/Remoting/Contexts/Context.hpp"
// self-typedef'd in il2cpp-class-internals.h
struct Il2CppAppContext : public System::Runtime::Remoting::Contexts::Context {};
NEED_NO_BOX(Il2CppAppContext);

#include "System/Runtime/Remoting/Messaging/AsyncResult.hpp"
// self-typedef'd in il2cpp-api-types.h
struct Il2CppAsyncResult : public System::Runtime::Remoting::Messaging::AsyncResult {};
NEED_NO_BOX(Il2CppAsyncResult);

// TODO: Il2CppCallType which "is a copy of System.Runtime.Remoting.Messaging.CallType" enum

// TODO: Il2CppMethodMessage is System::Runtime::Remoting::Messaging::MonoMethodMessage minus 4 fields at the end?

#include "System/Text/StringBuilder.hpp"
typedef System::Text::StringBuilder Il2CppStringBuilder;

#include "System/Threading/InternalThread.hpp"
typedef System::Threading::InternalThread Il2CppInternalThread;

#include "System/Threading/Thread.hpp"
// self-typedef'd in il2cpp-api-types.h
struct Il2CppThread : public System::Threading::Thread {};

#ifndef _HAD_CODEGEN_FIELDS
#undef USE_CODEGEN_FIELDS
#endif

#undef _HAD_CODEGEN_FIELDS

#else
// From Runtime.cpp (some may need the * removed):
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppMulticastDelegate*, multicastdelegate);
NEED_NO_BOX(Il2CppMulticastDelegate);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppAsyncCall*, async_call);
NEED_NO_BOX(Il2CppAsyncCall);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppInternalThread*, internal_thread);
NEED_NO_BOX(Il2CppInternalThread);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppReflectionEvent*, event_info);
NEED_NO_BOX(Il2CppReflectionEvent);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppStringBuilder*, stringbuilder);
NEED_NO_BOX(Il2CppStringBuilder);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppStackFrame*, stack_frame);
NEED_NO_BOX(Il2CppStackFrame);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppReflectionAssemblyName*, assembly_name);
NEED_NO_BOX(Il2CppReflectionAssemblyName);
// DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppReflectionAssembly*, assembly);
#ifndef UNITY_2021
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppReflectionAssembly*, mono_assembly);
#endif
NEED_NO_BOX(Il2CppReflectionAssembly);
#ifndef UNITY_2021
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppReflectionField*, mono_field);
#endif
NEED_NO_BOX(Il2CppReflectionField);
// DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppReflectionParameter*, parameter_info);
#ifndef UNITY_2021
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppReflectionParameter*, mono_parameter_info);
#endif
NEED_NO_BOX(Il2CppReflectionParameter);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppReflectionModule*, module);
NEED_NO_BOX(Il2CppReflectionModule);
#ifndef UNITY_2021
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppReflectionPointer*, pointer);
#endif
NEED_NO_BOX(Il2CppReflectionPointer);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppSystemException*, system_exception);
NEED_NO_BOX(Il2CppSystemException);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppArgumentException*, argument_exception);
NEED_NO_BOX(Il2CppArgumentException);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppMarshalByRefObject*, marshalbyrefobject);
NEED_NO_BOX(Il2CppMarshalByRefObject);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppSafeHandle*, safe_handle);
NEED_NO_BOX(Il2CppSafeHandle);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppSortKey*, sort_key);
NEED_NO_BOX(Il2CppSortKey);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppErrorWrapper*, error_wrapper);
NEED_NO_BOX(Il2CppErrorWrapper);
// TODO: attempt to move out of this conditional if codegen ever gets an Il2CppComObject?
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppComObject*, il2cpp_com_object);
NEED_NO_BOX(Il2CppComObject);
#endif
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppDelegate*, delegate);
NEED_NO_BOX(Il2CppDelegate);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppReflectionMonoType*, monotype);
NEED_NO_BOX(Il2CppReflectionMonoType);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppThread*, thread);
NEED_NO_BOX(Il2CppThread);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppReflectionRuntimeType*, runtimetype);
NEED_NO_BOX(Il2CppReflectionRuntimeType);
#ifndef UNITY_2021
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppReflectionMonoEventInfo*, mono_event_info);
#endif
NEED_NO_BOX(Il2CppReflectionMonoEventInfo);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppTypedRef*, typed_reference);
NEED_NO_BOX(Il2CppTypedRef);
#ifndef UNITY_2021
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppReflectionMethod*, mono_method);
#endif
NEED_NO_BOX(Il2CppReflectionMethod);
#ifndef UNITY_2021
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppMethodInfo*, mono_method_info);
#endif
NEED_NO_BOX(Il2CppMethodInfo);
#ifndef UNITY_2021
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppPropertyInfo*, mono_property_info);
#endif
NEED_NO_BOX(Il2CppPropertyInfo);
DEFINE_IL2CPP_DEFAULT_TYPE(Il2CppException*, exception);
NEED_NO_BOX(Il2CppException);

DEFINE_IL2CPP_ARG_TYPE(long double, "System", "Decimal");

template<class T, class Ptr>
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<ListWrapper<T, Ptr>> {
    static inline Il2CppClass* get() {
        static auto klass = ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<List<T>*>::get();
        return klass;
    }
};

#include "utils/Il2CppHashMap.h"
#include "utils/HashUtils.h"
#include "utils/StringUtils.h"

struct NamespaceAndNamePairHash
{
    size_t operator()(const std::pair<const char*, const char*>& pair) const
    {
        return il2cpp::utils::HashUtils::Combine(il2cpp::utils::StringUtils::Hash(pair.first), il2cpp::utils::StringUtils::Hash(pair.second));
    }
};

struct NamespaceAndNamePairEquals
{
    bool operator()(const std::pair<const char*, const char*>& p1, const std::pair<const char*, const char*>& p2) const
    {
        return !strcmp(p1.first, p2.first) && !strcmp(p1.second, p2.second);
    }
};

struct Il2CppNameToTypeHandleHashTable : public Il2CppHashMap<std::pair<const char*, const char*>, Il2CppMetadataTypeHandle, NamespaceAndNamePairHash, NamespaceAndNamePairEquals>
{
    typedef Il2CppHashMap<std::pair<const char*, const char*>, Il2CppMetadataTypeHandle, NamespaceAndNamePairHash, NamespaceAndNamePairEquals> Base;
    Il2CppNameToTypeHandleHashTable() : Base()
    {
    }
};

typedef struct Il2CppImageGlobalMetadata
{
    TypeDefinitionIndex typeStart;
    TypeDefinitionIndex exportedTypeStart;
    CustomAttributeIndex customAttributeStart;
    MethodIndex entryPointIndex;
    const Il2CppImage* image;
} Il2CppImageGlobalMetadata;

#pragma pack(pop)

#endif /* TYPEDEFS_H */
