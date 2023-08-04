#include <iostream>
#include <chrono>
#include <thread>
#include <cassert>
#include <cstring>
#include <maths.h>

#include <library_loader.h>

#if 0

// Required for determining when a library was last updated.
// Maintains paths of the true library file and the temp file.
struct DynamicLibrary
{
    void*   handle;
    size_t  last_file_time;

    char    file_path[256];
    char    temp_path[256];
};

static inline void get_base_path(char* buffer, size_t buffer_size);
static inline void set_path_from_base_path(char* buffer, size_t buffer_size, const char* path);

static inline size_t get_last_modified_time(const char* file_path);
static inline size_t get_file_size(const char* file_path);

static inline bool load_library_instance(DynamicLibrary* library, const char* library_path,
        const char* temporary_path);

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
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

static inline size_t
get_last_modified_time(const char* file_path)
{
    struct stat file_attributes = {};
    stat(file_path, &file_attributes);
    return (size_t)file_attributes.st_mtime;
}

static inline size_t
get_file_size(const char* file_path)
{
    struct stat file_attributes = {};
    stat(file_path, &file_attributes);
    return (size_t)file_attributes.st_size;
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

static inline bool
load_library_instance(DynamicLibrary* library, const char* library_path,
        const char* temporary_path)
{
    
    // Ensure that the user actually gave us a library.
    assert(library != NULL);

    // First, determine if the live version already exists, close it and delete.
    if ((get_file_size(temporary_path) != 0) && (library->handle))
    {
        dlclose(library->handle);
        unlink(library->temp_path);
    }

    // Now create a copy.
    size_t library_file_size = get_file_size(library_path);
    void* library_file_buffer = malloc(library_file_size);
    // TODO(Chris): We shouldn't be using malloc for this, instead we should
    // use some sort of OS level virtual allocation routine to load in large files.
    
    int library_fd = open(library_path, O_RDONLY);
    if (library_fd == -1) return false;

    read(library_fd, library_file_buffer, library_file_size);
    close(library_fd);

    int temporary_fd = open(temporary_path, O_WRONLY|O_CREAT|O_TRUNC, 00755);
    if (temporary_fd == -1) return false;

    write(temporary_fd, library_file_buffer, library_file_size);
    close(temporary_fd);
    free(library_file_buffer);

    // Now open the live version and then return.
    library->handle = dlopen(temporary_path, RTLD_NOW);
    library->last_file_time = get_last_modified_time(library_path);

    if (library->handle == NULL)
        return false;

    return true;

}

void*
get_proc_address(void* module_handle, const char* proc_name)
{
    void* proc_address = dlsym(module_handle, proc_name);
    return proc_address;
}
#endif

#endif 

// Extensions differ on platforms, so we need to define them here.
#if defined(_WIN32)
#   define LIBRARY_EXTENSION ".dll"
#elif defined(__linux__)
#   define LIBRARY_EXTENSION ".so"
#endif

int
main(int argc, char** argv)
{

    // Create our dynamic library structure and initialize our paths.
    // We store these paths within the dynamic_library_t, but that is not a requirement
    // We do this only for data coupling purposes--a dynamic library path is directly
    // related to the library itself. The front-end API doesn't care if these are filled
    // out and will never be checked or used.
    dynamic_library_t hotmath_library = {};
    std::string root_directory = get_canonical_executable_directory();
    std::string library_name = std::string("Hotmaths") + LIBRARY_EXTENSION;
    std::string templib_name = std::string("Hotmaths_live") + LIBRARY_EXTENSION;
    hotmath_library.lib_path = set_canonical_file_path(root_directory, library_name);
    hotmath_library.tmp_path = set_canonical_file_path(root_directory, templib_name);

    // Print our pathing schemes for reference.
    std::cout << "Root: " << root_directory << std::endl
        << "Library path: " << hotmath_library.lib_path << std::endl
        << "Temp Library Path: " << hotmath_library.tmp_path << std::endl;

    // Attempt to intialize our library instance. We use c-strings in this situation
    // because the platform functionality expects c-strings for their parameters.
    // It should also be noted that although we filled out the paths within the
    // dynamic_library_t structure and could use them within the function itself, this
    // may not be entirely apparent to front-end API users; only that when this
    // function succeeds, the structure contains internal data that manages where that
    // shared library is stored and the most recent file time.
    bool load_library_result = load_library_instance(&hotmath_library,
            hotmath_library.lib_path.c_str(), hotmath_library.tmp_path.c_str());
    assert(load_library_result == true);

    // Now that the library is loaded, we can now call the library's initialization
    // procedure as defined in its header file.
    bool init_library_result = init_hotmaths_library(hotmath_library.handle,
            get_library_proc);
    assert(load_library_result == true);

#if 0
    // Initialize our library instance.
    DynamicLibrary hotmaths_library = {};
    get_base_path(hotmaths_library.file_path, 256);
    get_base_path(hotmaths_library.temp_path, 256);
    set_path_from_base_path(hotmaths_library.file_path, 256, "Hotmaths.so");
    set_path_from_base_path(hotmaths_library.temp_path, 256, "Hotmaths_live.so");

    if (!load_library_instance(&hotmaths_library,
                hotmaths_library.file_path, hotmaths_library.temp_path))
    {
        assert(!"Unable to load the library into memory!");
    }

    // N    ow actually initialize the library.
    init_hotmaths_library(hotmaths_library.handle, get_proc_address);
#endif

    // A simulated "main loop" which does "stuff".
    static bool runtime_flag = true;
    while (runtime_flag == true)
    {
#if 0
        // We will now check the current file time of the library. If the file
        // time is different, then we know that the file was recomp'd.
        size_t current_file_time = get_last_modified_time(hotmaths_library.file_path);
        if (current_file_time != hotmaths_library.last_file_time)
            load_library_instance(&hotmaths_library,
                    hotmaths_library.file_path,
                    hotmaths_library.temp_path);

        std::cout << "Operation result: "
            << hotmath_perform_operation(3, 4)
            << std::endl;

#endif
        // Check the file time and update accordingly.
        // If the filetimes change, then we know the library should be reloaded.
        // Make changes to maths.cpp, compile, and it should update immediately!
        if (is_library_updated(hotmath_library.lib_path.c_str(), hotmath_library.file_time))
            load_library_instance(&hotmath_library, hotmath_library.lib_path.c_str(),
                    hotmath_library.tmp_path.c_str());

        // Perform the operation. This is how you will see the updates.
        std::cout << "Operation result: "
            << hotmath_perform_operation(3, 4)
            << std::endl;

        // Print some debug information that the loop is doing stuff.
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    return 0;
}

