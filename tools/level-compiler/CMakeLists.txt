cmake_minimum_required(VERSION 3.20)
project(zwodee-level-compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch nlohmann/json for robust level parsing
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# Create compiler executable
add_executable(zwodee-level-compiler source/level-compiler.cpp)

# Include shared engine headers to reference level-format.hpp
target_include_directories(zwodee-level-compiler
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(zwodee-level-compiler
    PRIVATE nlohmann_json::nlohmann_json
)

# Output build result to tools/ folder
set_target_properties(zwodee-level-compiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tools/"
)
