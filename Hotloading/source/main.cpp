#include <iostream>
#include <chrono>
#include <thread>
#include <cassert>
#include <cstring>
#include <maths.h>

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


int
main(int argc, char** argv)
{

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

    // Now actually initialize the library.
    init_hotmaths_library(hotmaths_library.handle, get_proc_address);
    
    // A simulated "main loop" which does "stuff".
    static bool runtime_flag = true;
    while (runtime_flag == true)
    {

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

        // Print some debug information that the loop is doing stuff.
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    return 0;
}

