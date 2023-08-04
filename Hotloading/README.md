# DLL / Shared Library Hotloading

Hotloading is a very simple, yet powerful construct rarely utilized for C and C++ applications.
This demonstration highlights the capabilities of hotloading by allowing the user to define a set front-end API
which dynamically loads itself into memory every time it detects a change (a compilation, in this case).

### Implementation Details

Typically, we link shared libraries at compile time which reduces the size
of the executable by letting external library code sit somewhere else on the OS.
Many executables can share one shared library this way.

Hotloading works by using the ability to dynamically load shared libraries within
code rather than linking them at compile time. 

The steps to load shared libraries dynamically is fairly straight forward; most
operating systems are able to do this in 1-2 lines code. However, for hotloading,
we need to perform some additional steps to make it work.

1. First, we need to establish an absolute pathing scheme to our executable directory.
Operating systems usually offer a way to get the path of the executable. You might be
thinking that the first argument of `argv` in `int main(int argc, char* argv[])`
contains this path. This is not always true; this path corresponds to the *calling
directory*, not the executable's directory. This path will change if you invoke an
executable from a different path than the one the executable is in.

2. Once we have this path, we can now construct a *relative path* to our shared library
from this absolute path. You will need to create two paths, one path is the actual
path to your shared library, and another path to a temporary library that may or may
not exist.

3. At this point, you will want to copy the actual library to the temporary library
location. This temporary library is what we will load into memory. Whenever we compile
or make changes to the actual library, we may need complete write-access to that file.
In some cases, write-access is revoked when the library is loaded into memory, so we
will need to create a temporary library file that we load in to allow for it to be
overwritten.

4. You will then load the temporary library using the OS calls like `LoadModuleA`
or `dlopen`, depending on whether you're using Windows or a Unix based system.

5. Once it is open, you will need to capture the last write time of the time. This
is what we are checking every loop cycle. If the actual library is updated, then we
re-run the load library procedure in step 4.

<br>

The library should provide a way to load its own function pointers. This will decouple
it from the executable should the API change. Generally speaking, if the front-end
API changes, you won't be able to hotload the library; the executable itself is not
"hotloadable" in the conventional sense (unless you're *really* clever).

**Helpful Notes:**

- The library you are hotloading has its own stack memory space, but it shares the
same heap as the executable. If you want some state to persist between the executable
and the library, then you will need the executable to manage the state through the heap.

- You may want to consider adding functionality to capture when a library is hotloaded,
such as `on_library_reload` and `on_library_unload` to perform init and cleanup
procedures. You will call these functions when you are about to unload the library
and when the library is successfully reloaded.

