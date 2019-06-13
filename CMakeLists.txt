cmake_minimum_required(VERSION 3.13)
project(pljit)

if (NOT UNIX)
    message(FATAL_ERROR "Unsupported OS")
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(address-sanitizer)
include(undefined-sanitizer)
include(clang-tidy)

find_package(GTest)

if (NOT GTEST_FOUND)
    message(STATUS "Adding GTest as external project")
    include(googletest)
endif ()

add_custom_target(lint)

add_subdirectory(pljit)
add_subdirectory(test)
