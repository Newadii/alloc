cmake_minimum_required(VERSION 3.8)
project(alloc)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
add_executable(alloc ${SOURCE_FILES})