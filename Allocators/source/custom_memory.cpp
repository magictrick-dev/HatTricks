#include <windows.h>
#include "custom_memory.h"

static inline size_t
get_nearest_page_granularity_size(size_t size_request)
{

    static size_t page_granularity = 0;
    if (page_granularity == 0)
    {
        SYSTEM_INFO system_info = {};
        GetSystemInfo(&system_info);
        page_granularity = system_info.dwAllocationGranularity;
    }

    // Determine the number of pages we need to allocate.
    size_t page_count = (size_request / page_granularity);
    if (size_request % page_granularity != 0)
        page_count++;

    size_t actual_allocation_size = page_count * page_granularity;
    return actual_allocation_size;

}

void
arena_pop(memory_arena_t *arena, size_t size)
{

    if (arena->commit < size)
        arena->commit = 0;
    arena->commit -= size;
    return;

}

void*
arena_push(memory_arena_t *arena, size_t size)
{
    
    void* offset = ((char*)arena->memory_region) + arena->commit;
    arena->commit += size;
    return offset;

}

bool 
allocate_arena(memory_arena_t *arena, size_t size_request)
{

    // Determine the maximum size.
    size_t size_maximum = get_nearest_page_granularity_size(size_request);

    // Allocate the region first.
    void* memory_ptr = VirtualAlloc(NULL, size_maximum,
            MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    if (memory_ptr == NULL)
        return false;

    // Now inspect the allocation and determine the actual size.
    size_t size_actual = 0;
    MEMORY_BASIC_INFORMATION memory_information = {};
    VirtualQuery(memory_ptr, &memory_information, sizeof(MEMORY_BASIC_INFORMATION));
    size_actual = (size_t)memory_information.RegionSize;

    // Fill out the memory arena struct.
    arena->memory_region = memory_ptr;
    arena->commit = 0;
    arena->capacity = size_actual;

    return true;

}


