#ifndef HOTMATHS_MATHS_H
#define HOTMATHS_MATHS_H

#if defined( _WIN32 )
#   define HOTMATHS_API_EXPORTS extern "C" __declspec( dllexport )
#else
#   define HOTMATHS_API_EXPORTS extern "C"
#endif

// Library definitions.
#define DEFINE_PERFORM_OPERATION(name) int name(int a, int b)
typedef DEFINE_PERFORM_OPERATION((*hotmath_perform_operation_fptr));
hotmath_perform_operation_fptr hotmath_perform_operation;

// Loader utilities.
typedef void* (*hotmaths_proc_loader)(void* handle, const char* fname);

// Loader definition.
bool
init_hotmaths_library(void* handle, hotmaths_proc_loader get_proc)
{

    if (!handle)
        return false;

    if (!(hotmath_perform_operation = (hotmath_perform_operation_fptr)
                get_proc(handle, "perform_operation"))) return false;

    return true;

}

#endif
