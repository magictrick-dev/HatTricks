cmake_minimum_required(VERSION 3.21)

# --- Basic Parameters ---------------------------------------------------------

project(nonblockwindow)
set(PLATFORM_EXECUTABLE_NAME "nonblocking")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(EXECUTABLE_TYPE "")
if (WIN32)
    set(EXECUTABLE_TYPE WIN32)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
        VS_STARTUP_PROJECT ${PLATFORM_EXECUTABLE_NAME})
    if(MSVC)
        add_definitions(/MP)
    endif()
endif (WIN32)

# --- Platform Sources ---------------------------------------------------------

set (PLATFORM_SOURCES
    "./source/main.h"
    "./source/main.cpp"
)

add_executable(${PLATFORM_EXECUTABLE_NAME} ${EXECUTABLE_TYPE}
    ${PLATFORM_SOURCES}
)

# --- Include Directories ------------------------------------------------------

set (COMMON_INCLUDES
    "./source"
)

target_include_directories(${PLATFORM_EXECUTABLE_NAME} PUBLIC ${COMMON_INCLUDES})
