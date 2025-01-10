cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(cpp_synchronization)

include(include/Utils/cmake/utils.cmake)

# Needed for colorful output
zephyr_compile_options(-fdiagnostics-color=always)

# Add the Edge Impulse SDK
add_subdirectory(edge-impulse-sdk/cmake/zephyr)

# include directories
set(INCLUDES
    .
    include
    include/Utils
    include/Drivers
    include/hal
    tflite-model
    model-parameters
    )
include_directories(${INCLUDES})
zephyr_include_directories(${INCLUDES})

# Wildcard append all source files
RECURSIVE_FIND_FILE(CPP_SOURCE_FILES_WILDCARD "src" "*.cpp")
RECURSIVE_FIND_FILE(C_SOURCE_FILES_WILDCARD "src" "*.c")
RECURSIVE_FIND_FILE(MODEL_FILES "tflite-model" "*.cpp")
list(APPEND SOURCE_FILES ${CPP_SOURCE_FILES_WILDCARD})
list(APPEND SOURCE_FILES ${C_SOURCE_FILES_WILDCARD})
list(APPEND SOURCE_FILES ${MODEL_FILES})

# add all sources to the project
target_sources(app PRIVATE ${SOURCE_FILES})
target_sources(app PRIVATE src/main.cpp)