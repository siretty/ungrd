cmake_minimum_required(VERSION 3.16)
project(ungrd)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external/abseil-cpp)

add_subdirectory(src)