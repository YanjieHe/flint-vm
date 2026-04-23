#ifndef FLINT_VM_TEST_H
#define FLINT_VM_TEST_H

#include "machine.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RUN_TEST(func)                                                         \
  do {                                                                         \
    printf("--- %s ---\n", #func);                                             \
    func();                                                                    \
  } while (0)

#define ASSERT_EQUAL(ACTUAL, EXPECTED)                                         \
  if ((ACTUAL) == (EXPECTED)) {                                                \
    total_tests++;                                                             \
    passed_tests++;                                                            \
    printf("  passed (line %d)\n", __LINE__);                                  \
  } else {                                                                     \
    total_tests++;                                                             \
    printf("  FAILED (line %d)\n", __LINE__);                                  \
  }

#define ASSERT_NOT_EQUAL(ACTUAL, EXPECTED)                                     \
  if ((ACTUAL) != (EXPECTED)) {                                                \
    total_tests++;                                                             \
    passed_tests++;                                                            \
    printf("  passed (line %d)\n", __LINE__);                                  \
  } else {                                                                     \
    total_tests++;                                                             \
    printf("  FAILED (line %d)\n", __LINE__);                                  \
  }

extern int total_tests;
extern int passed_tests;

void test_results_summary();

Program *create_program_with_single_function(const char *name, Byte *code,
                                             size_t code_length);

void copy_byte_code(Function *function, Byte *code, size_t code_length);

#endif /* FLINT_VM_TEST_H */