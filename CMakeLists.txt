cmake_minimum_required(VERSION 3.5)

project(flint-vm)

set(CMAKE_C_FLAGS "-std=c89")

include_directories(include/ libs/)

file(GLOB SOURCES "src/*.c")

add_executable(flint-vm ${SOURCES})