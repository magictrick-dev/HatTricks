#include <iostream>
#include <chrono>
#include <thread>
#include <cassert>
#include <cstring>
#include <maths.h>

// Required for determining when a library was last updated.
struct DynamicLibrary
{
    void*   handle;
    size_t  last_file_time;
};

// Definitions that must be defined by the platform.
static inline void get_base_path(char* buffer, size_t buffer_size);
static inline void set_path_from_base_path(char* buffer, size_t buffer_size, const char* path);
static inline size_t get_last_modified_time(const char* file_path);
static inline DynamicLibrary load_module_handle(const char* file_path);
void* get_proc_address(void* module_handle, const char* proc_name);


static inline void
set_path_from_base_path(char* buffer, size_t buffer_size, const char* path)
{
    // Base path should always be null-terminated.
    // TODO(Chris): We will need a safer way of accounting for string sizes here,
    // as we can't rely on the fact that the buffer is null-terminated. For now,
    // we can trust that it is given that our source is from get_base_path().
    size_t path_length = strlen(path);   
    size_t base_length = strlen(buffer);

    assert(path_length + path_length < buffer_size - 1); // Room for null-term.
    
    memcpy(buffer+base_length, path, base_length);
    buffer[base_length+path_length+1] = '\0'; // Null-term.

}

#if defined(__linux__)
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>

static inline size_t
get_last_modified_time(const char* file_path)
{
    struct stat file_attributes = {};
    stat(file_path, &file_attributes);
    return (size_t)file_attributes.st_mtime;
}

static inline void
get_base_path(char* buffer, size_t buffer_size)
{
    // Ensure it is zero'd for use.
    memset(buffer, 0x00, buffer_size);   

    // Attempt to pull the path out.
    int read_link_status = readlink("/proc/self/exe", buffer, buffer_size);
    assert(read_link_status != -1);

    // We now need to locate the last slash.
    size_t last_slash = buffer_size; // Pre-set to a fail condition.
    size_t index = 0;
    while (buffer[index] != '\0')
    {
        if (buffer[index] == '/')
            last_slash = index;
        index++;
    }
    
    // Our last slash should always be less then our buffer size.
    assert(last_slash+1 < buffer_size); // +1 for null-term pos.
    buffer[last_slash+1] = '\0'; // This is where we want the string to end.

}

static inline DynamicLibrary
load_module_handle(const char* module_path)
{
    DynamicLibrary result = {};

    result.handle = dlopen(module_path, RTLD_NOW);
    result.last_file_time = get_last_modified_time(module_path);
    return result;
}

void*
get_proc_address(void* module_handle, const char* proc_name)
{
    void* proc_address = dlsym(module_handle, proc_name);
    return proc_address;
}
#endif


int
main(int argc, char** argv)
{

    // Print the base path.
    std::cout << "Calling path: " << argv[0] << std::endl;

    // Attempt to retrieve the base path of the executable.
    char base_path_buffer[256];
    get_base_path(base_path_buffer, 256);
    std::cout << "Base path: " << base_path_buffer << std::endl;
    
    set_path_from_base_path(base_path_buffer, 256, "Hotmaths.so");
    std::cout << "Request path: " << base_path_buffer << std::endl;

    // Attempt to open the library module.
    DynamicLibrary hotmaths_library = load_module_handle(base_path_buffer);
    assert(hotmaths_library.handle != NULL);

    // Now print the last modified time.
    std::cout << "Last modified time: " << hotmaths_library.last_file_time << std::endl;

    // Now attempt to load the library.
    if (!init_hotmaths_library(hotmaths_library.handle, get_proc_address))
    {
        std::cout << "Unable to init library." << std::endl;
        return 1;
    }

    int a;
    int b;
    a = 4;
    b = 3;

    // A simulated "main loop" which does "stuff".
    static bool runtime_flag = true;
    while (runtime_flag == true)
    {
        
        std::cout << "Operation result: " << hotmath_perform_operation(a,b) << std::endl;

        // Print some debug information that the loop is doing stuff.
        std::cout << "Iteration Complete (@ Every 3 Seconds)" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    return 0;
}

