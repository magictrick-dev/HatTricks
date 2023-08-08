#ifndef CUSTOM_ALLOCATORS_MEMORY_H 
#define CUSTOM_ALLOCATORS_MEMORY_H 

struct memory_arena_t
{
    void* memory_region;
    size_t capacity;
    size_t commit;
};

bool   allocate_arena(memory_arena_t *arena, size_t request_size);
void*  arena_push(memory_arena_t *arena, size_t size);
void   arena_pop(memory_arena_t *arena, size_t size);
#define arena_push_struct(arena, type) ((type*)arena_push(arena, sizeof(type)))
#define arena_push_array(arena, type, count) ((type*)arena_push(arena, sizeof(type)*count))


#endif
