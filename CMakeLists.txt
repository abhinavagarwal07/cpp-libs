cmake_minimum_required(VERSION 3.0)
project(lib)

set(CMAKE_CXX_STANDARD 20)

add_executable(any any_test.cpp)
add_executable(optional optional_test.cpp)
