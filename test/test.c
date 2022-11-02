#include "test.h"
#include <stdlib.h>
#include <string.h>

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

  program = create_program("Program", 0, 0, 1, 0, 0);

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