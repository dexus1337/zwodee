cmake_minimum_required(VERSION 3.20)

project(zwodee VERSION 1.0.0 LANGUAGES C CXX)

# Global C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform Detection Definitions
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_definitions(ZWODEE_PLATFORM_WINDOWS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_definitions(ZWODEE_PLATFORM_LINUX)
endif()

if(CMAKE_CXX_FLAGS MATCHES "ZWODEE_PLATFORM_LINUX")
    link_libraries(dl pthread rt)
endif()

# Debug/Release Macros
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_definitions(ZWODEE_BUILD_DEBUG)
else()
    add_compile_definitions(ZWODEE_BUILD_RELEASE)
endif()

# Disable annoying warnings matching adam project style
if(MSVC)
    add_compile_options(/wd4251) # exporting std lib classes in DLLs when using MSVC
    add_compile_options(/wd4996) # use of std::xxx (deprecated/dangerous functions)
endif()

# Find SDL3 dependency
find_package(SDL3 REQUIRED)

# Grab all source files for the engine and tests automatically
file(GLOB_RECURSE engine_source_files "source/*.cpp")
file(GLOB_RECURSE test_source_files   "test/*.cpp")

# Remove test sources from main engine library to avoid duplicate symbols or main redefined
list(FILTER engine_source_files EXCLUDE REGEX "source/main.cpp")

# Create the shared library target for the engine
add_library(zwodee SHARED ${engine_source_files})

# This tells other targets where to find headers when they link to 'zwodee'
target_include_directories(zwodee 
    PUBLIC  include/    # Shared with others (SDK extraction)
    PRIVATE source/     # Internal to this lib
)

target_link_libraries(zwodee
    PUBLIC SDL3::SDL3
)

target_compile_definitions(zwodee
    PRIVATE ZWODEE_EXPORTS
)

# Set the output directory for the engine shared library
set_target_properties(zwodee PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
    PDB_OUTPUT_DIRECTORY     "${CMAKE_BINARY_DIR}/bin/"
)

# Create the test application
include(cmake/gtest.cmake)
enable_testing()

add_executable(zwodee-test ${test_source_files} ${engine_source_files})

target_include_directories(zwodee-test 
    PRIVATE include/    # Test doesn't need to share headers with others
)

target_compile_definitions(zwodee-test
    PRIVATE ZWODEE_EXPORTS
)

target_link_libraries(zwodee-test 
    PRIVATE SDL3::SDL3
)

# Set the output directory for the executable
set_target_properties(zwodee-test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/"
)

setup_gtest(zwodee-test)

# Build Level Compiler Tool
add_subdirectory(tools/level-compiler)
add_subdirectory(tools/texture-compiler)

