// --- Executable --------------------------------------------------------------
//
// Provides the load routine.
//

#include <windows.h>
#include <memory_module.h>
#include <cstdio>

typedef int (*library_add_numbers_fptr)(int, int);

library_add_numbers_fptr add_numbers = NULL;

int WINAPI
wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

    // --- Directory Setup -----------------------------------------------------
    //
    // In order to load the library into memory, we need to do some good ol'
    // Win32 programming to determine where everything is, local to the executable
    // path. This generally sucks with C++ standard library, but whatever.
    //
    
    char library_path[MAX_PATH];
    GetModuleFileNameA(NULL, library_path, MAX_PATH);

    // Convert the file path to the root directory of the executable.
    size_t slash_idx    = 0;
    size_t path_idx     = 0;
    size_t path_len     = 0;
    while (library_path[path_idx] != '\0')
    {
        if (library_path[path_idx] == '\\') slash_idx = path_idx;
        path_idx++;
    }

    library_path[++slash_idx] = '\0';
    path_len = slash_idx;

    // Now append the library name to the path.
    const char *library_name    = "testlib.dll";
    size_t      library_len     = strlen(library_name);

    if (library_len + path_len >= MAX_PATH)
    {
        MessageBoxA(NULL, "Unable to construct library path...", "Error", MB_OK);
        return 1;
    }

    for (size_t idx = 0; idx < library_len; ++idx)
    {
        library_path[slash_idx + idx] = library_name[idx];
    }

    library_path[slash_idx + library_len] = '\0';

    // --- Load Routine --------------------------------------------------------
    //
    // We now just need to load the whole DLL file into memory. Simple enough.
    //

    // Opent he file.
    HANDLE library_handle = CreateFileA(library_path, GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (library_handle == INVALID_HANDLE_VALUE)
    {
        MessageBoxA(NULL, "Unable to open library file...", "Error", MB_OK);
        return 1;
    }

    // Get the file size.
    LARGE_INTEGER library_size_li = {};
    GetFileSizeEx(library_handle, &library_size_li);
    size_t library_size = (size_t)library_size_li.QuadPart;

    // Allocate a buffer for the file.
    void *library_buffer = VirtualAlloc(NULL, (DWORD)library_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    // Now that we have the buffer, let's read the file into memory.
    DWORD bytes_read = 0;
    ReadFile(library_handle, library_buffer, (DWORD)library_size, &bytes_read, NULL);

    if (bytes_read != (DWORD)library_size)
    {
        MessageBoxA(NULL, "Unhandled case, unable to read entire file into memory...", "Error", MB_OK);
        return 1;
    }

    // --- Load Library --------------------------------------------------------
    //
    // Now we actually see if we can load the library.
    //

    HMEMORYMODULE module_handle = MemoryLoadLibrary(library_buffer, library_size);
    VirtualFree(library_buffer, 0, MEM_RELEASE);

    add_numbers = (library_add_numbers_fptr)(MemoryGetProcAddress(module_handle, "add_numbers"));

    int result = add_numbers(2, 3);

    char string_buffer[256];
    sprintf(string_buffer, "The result of 2 + 3 is %i\n", result);
    OutputDebugStringA(string_buffer);

    return 0;
}
