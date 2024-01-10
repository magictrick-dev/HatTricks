#include <windows.h>
#include <string>

typedef int (*add_numbers_fptr)(int, int);
static add_numbers_fptr library_add_numbers = NULL;

static std::string
get_executable_directory()
{

    // Get the module path, cached if need be.
    static bool cached = false;
    static std::string file_path = "";
    if (!cached)
    {
        cached = true;

        char module_path[MAX_PATH];
        GetModuleFileNameA(NULL, module_path, MAX_PATH);

        size_t idx = 0;
        size_t lsl = 0;
        while (module_path[idx] != '\0')
        {
            if (module_path[idx] == '\\')
                lsl = idx;
            idx++;
        }

        module_path[lsl + 1] = '\0';
        file_path = module_path;
    }

    return file_path;
}

int
main(int argc, char **argv)
{

    // Get the executable directory and then from there, get our required directories.
    std::string root_directory = get_executable_directory();
    std::string library_directory = root_directory + "testlib.dll";

    // Once we have these directories, we can now attempt to load library into memory.
    HMODULE library_handle = LoadLibraryA(library_directory.c_str());
    if (library_handle == NULL) return 1;

    // Now let's get the addition procedure.
    library_add_numbers = (add_numbers_fptr)GetProcAddres(library_handle, "add_numbers");
    if (library_add_numbers == NULL) return 1;

    // Print the add numbers out.
    std::cout << "The sum of 2 and 3 is: " << library_add_numbers(2, 3) << std::endl;

    return 0;
}
