#ifndef FLINT_VM_TEST_H
#define FLINT_VM_TEST_H

#include "value.h"
#include <stdio.h>
#include "machine.h"

#define ASSERT_EQUAL(ACTUAL, EXPECTED)                                         \
  if ((ACTUAL) == (EXPECTED)) {                                                \
    total_tests++;                                                             \
    passed_tests++;                                                            \
    printf("Test case suceeded: \"%s\" (line %d)\n", __FUNCTION__, __LINE__);  \
  } else {                                                                     \
    total_tests++;                                                             \
    printf("Test case failed: \"%s\" (line %d)\n", __FUNCTION__, __LINE__);    \
  }

extern int total_tests;
extern int passed_tests;

void test_results_summary();

Program *create_program_with_single_function(const char *name, Byte *code,
                                             size_t code_length);

void load_program_on_machine(Program *program, Machine *machine,
                             int main_module_id, int main_function_id);

void copy_byte_code(Function *function, Byte *code, size_t code_length);

#endif /* FLINT_VM_TEST_H */