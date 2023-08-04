#ifndef LIBRARY_LOADER_H
#define LIBRARY_LOADER_H
#include <filesystem>
#include <string>
#include <cassert>
#include <cstring>

#if defined(_WIN32)
#   include <windows.h>
#endif

#if defined(__linux__)
#   include <dlfcn.h>
#   include <unistd.h>
#   include <fcntl.h>
#   include <sys/stat.h>
#endif

// The basic control structure which keeps track of the currently loaded dynamic library.
// We are storing the paths within the structure to determine where we are looking to
// find the libraries and where we want to store our temporary "live" version that is
// being used within the executable.
struct dynamic_library_t 
{
    void*   handle;
    size_t  file_time;
    
    std::string lib_path;
    std::string tmp_path;
};

// This will construct a path based on a canonical root directory as provided by
// get_canonical_executable_directory or by other means, and concatenate a relative
// path to that and return the result.
inline std::string
set_canonical_file_path(std::string root_directory, std::string relative_path)
{

    std::filesystem::path root_path = root_directory;
    root_path.append(relative_path);
    return root_path.string();

}

// This will fetch the canonical path of the executable and return the base directory
// it resides in. This is important because we can't rely on relative pathing schemas
// since we can't predict where the program is being invoked from. Using an absolute
// path allows us to then supply relative pathing from the perspective of the executable
// directory rather than the directory of the calling location.
inline std::string
get_canonical_executable_directory()
{

    std::string result = "";

    // Platform specific functions are needed to determine where the executable
    // currently resides absolutely.
#   if defined(_WIN32)
        char path_buffer[MAX_PATH];
        GetModuleFileNameA(NULL, path_buffer, MAX_PATH);
        result = path_buffer;
#   elif defined(__linux__)
        char path_buffer[256];
        int read_link_status = readlink("/proc/self/exe", path_buffer, 256);
        assert(read_link_status != -1);
        result = path_buffer;
#   endif

    // If it is empty, then this functionality was unimplemented.
    assert(result != "");
    
    // Now we need construct the base directory.
    result = std::filesystem::path(result).parent_path().string();
    return result;

}

// Returns the last write time of the provided file path given the file exists.
// There are more "C++" ways to do this, but to keep things at a numerical level,
// we will use OS-level facilities to retrieve file time.
inline size_t
get_library_file_time(const char* file_path)
{

    size_t file_time = 0;

#   if defined(_WIN32)
        // Welcome to Win32 programming, folks.
        HANDLE file_handle = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ,
                NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        assert(file_handle != INVALID_HANDLE_VALUE);
        FILETIME last_write_time;
        GetFileTime(file_handle, NULL, NULL, &last_write_time);
        LARGE_INTEGER quad_word_cast = {};
        quad_word_cast.LowPart = last_write_time.dwLowDateTime;
        quad_word_cast.HighPart = last_write_time.dwHighDateTime;
        file_time = (size_t)quad_word_cast.QuadPart;
        CloseHandle(file_handle);
#   elif defined(__linux__)
        struct stat file_attributes = {};
        stat(file_path, &file_attributes);
        file_time = (size_t)file_attributes.st_mtime;
#   endif

    return file_time;

}

// Used in the loop to determine if the library was updated. The last time
// is the time when library last loaded.
inline bool
is_library_updated(const char* file_path, size_t last)
{
    size_t current_file_time = get_library_file_time(file_path);
    if (current_file_time != last)
        return true;
    return false;
}

// This will attempt to load the procedures within the library. We provide
// this function to the library loader procedure provided in its header file.
// The library will check if the proc address returned by this function is valid.
// We will not need to check.
void*
get_library_proc(void* handle, const char* proc_name)
{
    void* proc_address = NULL;

#   if defined (_WIN32)
        proc_address = (void*)GetProcAddress((HMODULE)handle, proc_name);
#   elif defined (__linux__)
        proc_address = dlsym(handle, proc_name);
#   endif
    
    return proc_address;
}

// This function is responsible for loading a library into memory. It first checks
// if there is a library already loaded and unloads it. This will allow the user
// to call this function to "hotswap" the shared library if it is already in memory.
bool
load_library_instance(dynamic_library_t* library, const char* lib_path, const char* tmp_path)
{

    assert(library != NULL); // The user must provide a valid pointer.

    // Unload the library if it is already loaded.
    if (library->handle != NULL)
    {
#       if defined(_WIN32)
            FreeLibrary((HMODULE)library->handle);
#       elif defined(__linux)
            dlclose(library->handle);
#       endif

        library->handle = NULL; // Zero out, prevent hanging pointer refs.

    }
       
    // Check if the library itself exists.
    if (!std::filesystem::exists(lib_path)) return false;

    // Create a copy of the library. (That is one aggressive namespace!)
    if (!std::filesystem::copy_file(lib_path, tmp_path,
            std::filesystem::copy_options::overwrite_existing))
        return false;
    
    // Once the file is copied, we can now load it.
#   if defined(_WIN32)
        library->handle = (void*)LoadLibraryA(tmp_path);
#   elif defined(__linux)
        library->handle = dlopen(tmp_path, RTLD_NOW);
#   endif

    // Ensure the library is loaded.
    if (library->handle == NULL)
        return false;

    // Now get the file time.
    library->file_time = get_library_file_time(lib_path);

    return true;
}

#endif
