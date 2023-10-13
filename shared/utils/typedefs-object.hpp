#pragma once

// include order shenanigans to be able to use the object wrapper type as a base type
#if defined(USE_CODEGEN) && __has_include("System/Object.hpp")

// if we have System.Object, use ObjectWrapper as that so we get methods for free
#include "System/Object.hpp"
using ObjectWrapperType = ::System::Object;

#else

/// if not found or no codegen use is requested, just use the bs hook wrapper type instead
#include "utils/base-wrapper-type.hpp"
using ObjectWrapperType = ::bs_hook::Il2CppWrapperType;

#endif

// object wrapper type should still somewhere have a base of il2cppwrappertype
static_assert(std::is_base_of_v<::bs_hook::Il2CppWrapperType, ObjectWrapperType>, "ObjectWrapperType did not inherit Il2CppWrapperType somewhere down the line! this is a problem!");
