#include <iostream>
#include <chrono> // Used for sleeping.
#include <thread> // Used for sleeping.

#include <maths.h>
#include <library_loader.h>

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

    // A simulated "main loop" which does "stuff".
    static bool runtime_flag = true;
    while (runtime_flag == true)
    {
        
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

        // Sleep to prevent the output from being nuked.
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    return 0;
}

