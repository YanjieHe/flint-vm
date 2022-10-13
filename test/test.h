#ifndef FLINT_VM_TEST_H
#define FLINT_VM_TEST_H

#include "value.h"
#include <stdio.h>
#include "machine.h"

#define ASSERT_EQUAL(ACTUAL, EXPECTED)                                         \
  if ((ACTUAL) == (EXPECTED)) {                                                \
    total_tests++;                                                             \
    passed_tests++;                                                            \
  } else {                                                                     \
    total_tests++;                                                             \
    printf("Test case fails: \"%s\" (line %d)\n", __FUNCTION__, __LINE__);     \
  }

extern int total_tests;
extern int passed_tests;

void test_results_summary();

Program *create_program_with_single_function(Byte *code, size_t code_length);

void free_program_with_single_function(Program* program);

void load_program_on_machine(Program *program, Machine *machine,
                             int main_module_id, int main_function_id);

#endif /* FLINT_VM_TEST_H */