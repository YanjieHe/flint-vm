#include "test.h"
#include <stdlib.h>
#include <string.h>

#include "test_arithmetic.h"
#include "test_array.h"
#include "test_function.h"
#include "test_jump.h"
#include "test_structure.h"
#include "test_global_variable.h"
#include "test_closure.h"

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

  test_add();
  test_subtract();

  test_create_an_array();
  test_create_an_illegal_array();
  test_access_array();

  test_function_invocation();
  test_function_return_i32();
  test_function_factorial();
  test_function_native_function_call();
  test_function_tail_call();
  test_function_mutual_tail_recursion();

  test_jump();
  test_loop();

  test_structure();
  test_structure_get_field_value();

  test_global_variable();

  test_closure_invocation();
  test_function_reference_passing();

  test_results_summary();

  return 0;
}