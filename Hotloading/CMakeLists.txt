cmake_minimum_required(VERSION 3.21)

project(hotloading)
set(PLATFORM_EXECUTABLE_NAME "Hotloading")
set(PLATFORM_RUNTIME_NAME "Hotmaths")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)

add_executable(Hotloading
    "source/main.cpp"
)

add_library(Hotmaths SHARED
    "hotmaths/maths.cpp"
)

set(EXECUTABLE_TYPE "")
if (WIN32)
    set(EXECUTABLE_TYPE WIN32)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
        VS_STARTUP_PROJECT ${PLATFORM_EXECUTABLE_NAME})
endif (WIN32)

if (LINUX)
    target_link_libraries(Hotloading dl)
endif(LINUX)

set_target_properties(${PLATFORM_RUNTIME_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set_target_properties(${PLATFORM_RUNTIME_NAME} PROPERTIES PREFIX "")
set_target_properties(${PLATFORM_RUNTIME_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

set(COMMON_SOURCES
    "./source"
    "./hotmaths"
)

target_include_directories(${PLATFORM_EXECUTABLE_NAME} PUBLIC ${COMMON_SOURCES})
target_include_directories(${PLATFORM_RUNTIME_NAME} PUBLIC ${COMMON_SOURCES})
