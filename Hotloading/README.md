# DLL / Share Library Hotloading

Hotloading is a very simple, yet powerful construct rarely utilized for C and C++ applications.
This demonstration highlights the capabilities of hotloading by allowing the user to define a set front-end API
which dynamically loads itself into memory every time it detects a change (a compilation, in this case).

### Implementation Notes

For hotloading to work, the shared library must be able to be written to. Since loading a library
into memory may potentially lock a DLL, the naive approach is to make a copy of the DLL in a
discrete directory or simply making a copy using a different name and loading that. This, while easy,
is not ideal since it effectively doubles the size of our shared library files when in use.

We may find it advantageous to take advantage of the fact that we can load DLLs into memory *manually*,
thus allowing us to leave the read/write protection of the file out of the picture. Essentially speaking,
if we pull the entire DLL into memory, the DLL file itself is free to be updated as much as we want.

This then leaves us with a dependency issue--how do we manage the code of the library API without directly
coupling it with the platform executable API? Realistically, we can follow the GLAD library's approach of
creating a header file filled with typedefs and function pointers that are filled out during the load process.
The platform's only responsibility is giving the API a means of actually *getting* to the DLL and fetch procs.
The front-end API of the library need only provide the "fetch function" that fills out the function pointers
for the platform.

### Initial Code Theory

```
// First, the library defines the functions which it
// provides to the front-end API. These are filled out
// by the "load_my_func_library" function provided by
// the library. Since every library is different, it is
// the library's responsibility to provide said interface
// to the user. Therefore, this method decouples dependency
// on the platform from doing all the function loading.
typedef void (*my_main_func_ptr)(my_state* state);
my_main_func_ptr my_main_func;

typedef b32 (*my_init_func_ptr)(my_state* state);
my_init_func_ptr my_init_func;


// These handles are from the OS platform, defined internally
// by the platform layer. The platform layer will then pass it
// to the "load_my_func_library"
typedef void* (*platform_get_interface_proc)(const char* proc_name);
typedef void* (*platform_get_interface_handle)(void* handle, const char* handle);

// This function is then pre-written in the library header.
// This header will then run the routine, defining the above
// function pointers as though they were functionsA.
b32 load_my_func_library(platform_get_interface_handle proc_func,
			 platform_get_interface_proc proc_handle)
{
	// Load the handle into memory using the platform provided function.
	void* proc_handle = proc_func("my_func_lib.dll");
	if (proc_handle == NULL) return MY_FUNC_FAILURE;
	
	// Load all the necessary functions into memory.
	if ((my_main_func = proc_func("my_main")) == NULL) return MY_FUNC_FAILURE;
	if ((my_init_func = proc_func("my_init")) == NULL) return MY_FUNC_FAILURE;

	// If we found all the funcs, then it worked. Success!
	return MY_FUNC_SUCCESS;
}
```
