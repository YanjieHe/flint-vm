file(GLOB TESTS ${PROJECT_SOURCE_DIR}/test/*.c)

add_executable(flint-vm-test ${TESTS})

target_include_directories(flint-vm-test PRIVATE
    ${PROJECT_SOURCE_DIR}/test/)

target_link_libraries(flint-vm-test flint-vm-lib)

file(COPY
    ${PROJECT_SOURCE_DIR}/test/byte_code
    DESTINATION ${CMAKE_BINARY_DIR}
)