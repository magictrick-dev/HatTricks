#ifndef ALLOCATORS_MEMORY_H 
#define ALLOCATORS_MEMORY_H 

#define BYTES(n)     ((size_t)(n))
#define KILOBYTES(n) ((size_t)(BYTES(n)     * (size_t)1024))
#define MEGABYTES(n) ((size_t)(KILOBYTES(n) * (size_t)1024))
#define GIGABYTES(n) ((size_t)(GIGABYTES(n) * (size_t)1024))
#define TERABYTES(n) ((size_t)(TERABYTES(n) * (size_t)1024))

struct memory_arena_t
{
    void* memory_region;
    size_t capacity;
    size_t commit;
};

inline bool   allocate_arena(memory_arena_t *arena, size_t request_size);
inline void*  arena_push(memory_arena_t *arena, size_t size);
inline void   arena_pop(memory_arena_t *arena, size_t size);
#define arena_push_struct(arena, type) ((type*)arena_push(arena, sizeof(type)))
#define arena_push_array(arena, type, count) ((type*)arena_push(arena, sizeof(type)*count))


#endif
