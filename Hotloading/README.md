# DLL / Share Library Hotloading

Hotloading is a very simple, yet powerful construct rarely utilized for C and C++ applications.
This demonstration highlights the capabilities of hotloading by allowing the user to define a set front-end API
which dynamically loads itself into memory every time it detects a change (a compilation, in this case).

### Implementation Details

In order to prevent tight coupling between the loading process or executable, the
library should define its own "loader" function. This loader function will automatically
fill out function stubs with the appropriate address into the library. For this to
work, the executable should provide the loader with a "get process" function which
gives the loader function a handle to its own library and a function which allows
it to search for its function addresses. How the executable performs this procedure
isn't relevant for the library.

In my test case, I created a library called "hotmaths" which simply defines a 
function that does an operation on two integers and returns an integer. The executable
executes this operation by supplying two numbers and and then spitting a result out
every 250ms. This will simulate a runtime loop. The 250ms interval prevents it from
nuking standard output with text.

The code is a little dense for what it is trying to accomplish--you can heavily
simplify this by using standard library facilities for string utilities, filepaths,
and file operations. The only facility that you *must* write using OS calls is the
library loader.

**The rough steps of what this is doing below:**

1. Main creates a "DynamicLibrary" structure.

2. Create two paths, one path to the shared library relative to the executable
and a second path to a "temporary" library location. This path shouldn't assume
that the user is running the program in the directory that the executable exists
in. Relative pathing will not work! You must get the executable location, and then
construct your relative pathing scheme from that.

    Executable path: `~Development/Project/bin/my_executable`

    Actual library path: `~/Development/Project/bin/my_library.so`

    Temporary library path: `~/Development/Project/bin/my_library_live.so`

3. You will now copy the actual library file to the temporary library location.
In some situations, overwriting a shared library while it is in use is not possible,
so we must create a "working copy" of the library that we can maintain in memory.
The compiler will attempt to overwrite the library file, which is what we *want* to
happen to allow for the hotloading functionality to work.

4. You will then need to capture the current file time at the time of it being loaded.
This time will be checked at iteration of the main loop.

5. When there is a time difference in the main loop, re-run the library load procedure.

**Some helpful notes for extending this beyond trivial use-cases:**

Your library uses its own memory space (but can reach out to the executable memory
space if you provide pointers), so use caution when handling hotloading. In such
cases when you want to preserve state, then your state should exist on the heap
with references maintained by the executable. This creates some coupling between
the executable and library, but is easy to manage when done correctly.

In the past, I have given a fixed-size permanent heap to the library which it uses
to maintain state through loads.

You will also want to create some functionality to perform some operations when
the library is being initialized, executed within a mainloop, shuts down (but not being
reloaded), on-reload, and on-unload. The last two are fairly important if you want
to do some heap initialization and heap cleanup.

