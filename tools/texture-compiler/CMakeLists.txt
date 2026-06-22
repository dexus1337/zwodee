cmake_minimum_required(VERSION 3.20)
project(zwodee-texture-compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch stb_image library
include(FetchContent)
FetchContent_Declare(
    stb
    URL https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
    DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif()

# Create compiler executable
add_executable(zwodee-texture-compiler 
    source/texture-compiler.cpp
    source/stb_impl.cpp
)

target_include_directories(zwodee-texture-compiler
    PRIVATE ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${stb_SOURCE_DIR}
)

# Output build result to tools/ folder
set_target_properties(zwodee-texture-compiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tools/"
)
