# DLL / Share Library Hotloading



### Theory

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
