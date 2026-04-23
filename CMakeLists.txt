cmake_minimum_required(VERSION 3.5)

project(flint-vm)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_subdirectory(src)
add_subdirectory(test)