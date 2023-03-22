cmake_minimum_required(VERSION 3.5)

project(flint-vm)

set(CMAKE_C_FLAGS "-std=c89")

add_subdirectory(src)

enable_testing()
add_subdirectory(test)