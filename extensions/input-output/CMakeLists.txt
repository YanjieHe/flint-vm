cmake_minimum_required(VERSION 3.5)

project(input-output)

set(CMAKE_C_FLAGS "-std=c89")

include_directories(include/ libs/ test/ ../../include)

file(GLOB SOURCES "src/*.c")
file(GLOB TESTS "test/*.c")

add_library(flint-vm-input-output SHARED ${SOURCES} ${TESTS})