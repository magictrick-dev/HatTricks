
extern "C" __declspec( dllexport ) int
perform_operation(int a, int b)
{
    int result = a + b;
    return result;
}

