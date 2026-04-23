file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)

add_library(flint-vm-lib STATIC ${SOURCES})

target_include_directories(flint-vm-lib PUBLIC
    ${PROJECT_SOURCE_DIR}/include/
    ${PROJECT_SOURCE_DIR}/libs/)

target_link_libraries(flint-vm-lib PUBLIC ${CMAKE_DL_LIBS})

target_compile_options(flint-vm-lib PRIVATE -pedantic-errors)

if(FLINT_VM_DEBUG_MODE)
    message(STATUS "FLINT_VM_DEBUG_MODE is ON")
    target_compile_definitions(flint-vm-lib PUBLIC FLINT_VM_DEBUG_MODE)
endif()

add_executable(flint-vm ${PROJECT_SOURCE_DIR}/main.c)
target_link_libraries(flint-vm flint-vm-lib)