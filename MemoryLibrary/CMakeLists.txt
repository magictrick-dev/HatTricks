cmake_minimum_required(VERSION 3.21)

# --- Configuration ------------------------------------------------------------

project(memlibs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# --- Build Setup --------------------------------------------------------------

add_executable(example WIN32
    "./executable/main.cpp"
    "./executable/memory_module.cpp"
    "./executable/memory_module.h"
)

add_library(testlib SHARED
    "./library/testlib.cpp"
)

# --- Additional Configuration Settings ----------------------------------------

set_target_properties(testlib PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set_target_properties(testlib PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(example PUBLIC "./executable")
target_include_directories(testlib PUBLIC "./library")
