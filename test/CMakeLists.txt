include_directories(
    ${PROJECT_SOURCE_DIR}/include/
    ${PROJECT_SOURCE_DIR}/libs/
    ${PROJECT_SOURCE_DIR}/test/)

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB TESTS ${PROJECT_SOURCE_DIR}/test/*.c)

add_executable(flint-vm-test ${SOURCES} ${TESTS})

target_link_libraries(flint-vm-test ${CMAKE_DL_LIBS})