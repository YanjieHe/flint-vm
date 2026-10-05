cmake_minimum_required(VERSION 3.5)

project(flint_io C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(flint_io SHARED
    src/flint_io.c
    ../../src/unicode.c
)

target_include_directories(flint_io PRIVATE include ../../include)
