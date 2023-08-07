cmake_minimum_required(VERSION 3.21)

project(allocators)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(allocators WIN32
    "./source/main.cpp"
    "./source/memory.cpp"
)

target_include_directories(allocators PUBLIC "./source")

