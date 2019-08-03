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


#if (NOT GTEST_FOUND)
#    message(STATUS "Adding GTest as external project")
#    include(googletest)
#endif ()


if (NOT GTEST_FOUND)
    message(STATUS "Adding bundled Google Test")
    set(BUILD_GMOCK OFF CACHE BOOL INTERNAL)
    set(INSTALL_GTEST OFF CACHE BOOL INTERNAL)

    add_subdirectory(thirdparty/google/googletest)

    add_library(GTest::GTest ALIAS gtest)
endif ()


add_custom_target(lint)

add_subdirectory(pljit)
add_subdirectory(test)