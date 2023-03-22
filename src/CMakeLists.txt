include_directories(
    ${PROJECT_SOURCE_DIR}/include/
    ${PROJECT_SOURCE_DIR}/libs/)

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)

add_executable(flint-vm ${SOURCES} ${PROJECT_SOURCE_DIR}/main.c)

target_link_libraries(flint-vm ${CMAKE_DL_LIBS})