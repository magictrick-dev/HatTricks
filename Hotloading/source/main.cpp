#include <iostream>
#include <chrono>
#include <thread>

int
main(int argc, char** argv)
{

    // A simulated "main loop" which does "stuff".
    static bool runtime_flag = true;
    while (runtime_flag == true)
    {
        
        // Print some debug information that the loop is doing stuff.
        std::cout << "Iteration Complete (@ Every 3 Seconds)" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    return 0;
}

