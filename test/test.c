#include "test.h"
#include <stdlib.h>
#include <string.h>

#include "test_arithmetic.h"
#include "test_array.h"
#include "test_closure.h"
#include "test_function.h"
#include "test_global_variable.h"
#include "test_jump.h"
#include "test_structure.h"

int total_tests;
int passed_tests;

void test_results_summary() {
  printf("total test cases: %d\n", total_tests);
  printf("passed test cases: %d\n", passed_tests);
}

Program *create_program_with_single_function(const char *name, Byte *code,
                                             size_t code_length) {
  Program *program;
  Function *entry;

  program = create_program("Program", 0, 0, 1, 0, 0, 0);

  entry = &(program->functions[0]);
  copy_byte_code(entry, code, code_length);
  entry->name = make_string(name);
  entry->constant_pool = NULL;
  entry->constant_pool_size = 0;
  entry->args_size = 0;
  entry->locals = 0;
  entry->stack = 0;
  program->entry = entry;

  return program;
}

void copy_byte_code(Function *function, Byte *code, size_t code_length) {
  function->code = malloc(sizeof(Byte) * code_length);
  memcpy(function->code, code, code_length);
  function->code_length = code_length;
}

int main(int argc, char **argv) {
  total_tests = 0;
  passed_tests = 0;

  RUN_TEST(test_add);
  RUN_TEST(test_subtract);

  RUN_TEST(test_create_an_array);
  RUN_TEST(test_create_an_illegal_array);
  RUN_TEST(test_access_array);

  RUN_TEST(test_function_invocation);
  RUN_TEST(test_function_return_i32);
  RUN_TEST(test_function_factorial);
  RUN_TEST(test_function_native_function_call);
  RUN_TEST(test_function_tail_call);
  RUN_TEST(test_function_mutual_tail_recursion);

  RUN_TEST(test_jump);
  RUN_TEST(test_loop);

  RUN_TEST(test_structure);
  RUN_TEST(test_structure_get_field_value);

  RUN_TEST(test_global_variable);

  RUN_TEST(test_closure_invocation);
  RUN_TEST(test_function_reference_passing);

  test_results_summary();

  return 0;
}