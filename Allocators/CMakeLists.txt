cmake_minimum_required(VERSION 3.21)

project(allocators)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(allocators WIN32
    "./source/main.cpp"
    "./source/custom_memory.cpp"
)

