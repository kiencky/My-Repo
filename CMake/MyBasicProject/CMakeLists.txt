# Set the minumum version of CMake that can be use
cmake_minimum_required(VERSION 3.28)

# Set the project name
project(
    MyLogger
    VERSION 1.0.0
    DESCRIPTION "My simply logger project"
    LANGUAGES CXX
    )

# Set C++ standard flag
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create build options
option(ENABLE_VERBOSE "Enable verbose logging" ON)
option(ENABLE_TEST "Enable build test" ON)

# Generate config file
configure_file(
    ${CMAKE_SOURCE_DIR}/config/config.h.in
    ${CMAKE_BINARY_DIR}/generate/config.h
)

# Add subdirectory
add_subdirectory(logger)
add_subdirectory(app)

if(ENABLE_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
