// --- Test Library ------------------------------------------------------------
//
// Defines some basic functionality that we will be importing back to our executable.
//

extern "C" __declspec(dllexport) int
add_numbers(int a, int b)
{
    return a + b;
}

