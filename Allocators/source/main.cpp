#include <windows.h>
#include <new>
#include "custom_memory.h"

class ShapeRectangle
{
    public:
        ShapeRectangle(int width, int height);
        ~ShapeRectangle();

        int calculate_area() const;

    private:
        int width;
        int height;
};

ShapeRectangle::
ShapeRectangle(int width, int height) : width(width), height(height)
{ }

ShapeRectangle::
~ShapeRectangle()
{ }

int ShapeRectangle::
calculate_area() const
{
    return width * height;
}

int WINAPI
wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

    memory_arena_t base_arena;
    allocate_arena(&base_arena, 1024);

    // Fill the arena up to our size.
    for (size_t i = 0; i < 256; i++)
    {
        int* my_int = (int*)arena_push(&base_arena, sizeof(int));
        *my_int = (i + 1) * 2;
    }

    // Now lets pop back.
    arena_pop(&base_arena, 1024);

    // What happens if we want to construct a class? Well... not straight forward
    // but there is a way! Placement new operator...
    void* rectangle_buffer = arena_push(&base_arena, sizeof(ShapeRectangle));
    ShapeRectangle *my_rec = ::new (rectangle_buffer) ShapeRectangle(3, 4);

    // Now we can use rectangle normally.
    int my_area = my_rec->calculate_area();

    // We must manually invoke the destructor before we can "pop"; there is no
    // such thing as "placement delete".
    my_rec->~ShapeRectangle();
    arena_pop(&base_arena, sizeof(ShapeRectangle)); // Now "delete" rectangle!

    return 0;
}

