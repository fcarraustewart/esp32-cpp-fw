cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(cpp_synchronization)

include(include/Utils/cmake/utils.cmake)

# Needed for colorful output
zephyr_compile_options(-fdiagnostics-color=always)

# include directories
set(INCLUDES
    .
    include
    include/Utils
    include/Drivers
    include/hal
    )
include_directories(${INCLUDES})
zephyr_include_directories(${INCLUDES})

# Wildcard append all source files
RECURSIVE_FIND_FILE(CPP_SOURCE_FILES_WILDCARD "src" "*.cpp")
RECURSIVE_FIND_FILE(C_SOURCE_FILES_WILDCARD "src" "*.c")
list(APPEND SOURCE_FILES ${CPP_SOURCE_FILES_WILDCARD})
list(APPEND SOURCE_FILES ${C_SOURCE_FILES_WILDCARD})

# add all sources to the project
target_sources(app PRIVATE ${SOURCE_FILES})
target_sources(app PRIVATE src/main.cpp)