cmake_minimum_required(VERSION 3.10)
project(chess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GTEST_ROOT "D:/cpp/libraries/googletest-1.15.2/googletest/include/gtest")
set(GTEST_INCLUDE_DIR "${GTEST_ROOT}/include")
set(GTEST_LIBRARY_DIR "${GTEST_ROOT}/lib")

include(CTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
