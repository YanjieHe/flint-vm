#include "test.h"
#include <stdlib.h>

int total_tests;
int passed_tests;

void test_results_summary() {
  printf("total test cases: %d\n", total_tests);
  printf("passed test cases: %d\n", passed_tests);
}

Program *create_program_with_single_function(const char *name, Byte *code,
                                             size_t code_length) {
  Program *program;

  program = malloc(sizeof(Program));

  program->file_name = NULL;
  program->module_count = 1;
  program->modules = malloc(sizeof(Module));
  program->structure_count = 0;
  program->structures_meta_data = NULL;
  init_module(&(program->modules[0]));
  program->modules[0].function_count = 1;
  program->modules[0].functions = malloc(sizeof(Function));
  copy_byte_code(&(program->modules[0].functions[0]), code, code_length);
  program->modules[0].functions[0].name = make_string(name);
  program->modules[0].functions[0].constant_pool = NULL;
  program->modules[0].functions[0].constant_pool_size = 0;
  program->entry = &(program->modules[0].functions[0]);

  return program;
}

void load_program_on_machine(Program *program, Machine *machine,
                             int main_module_id, int main_function_id) {
  machine->env.function =
      &(program->modules[main_module_id].functions[main_function_id]);
  machine->env.module = &(program->modules[main_module_id]);
}

void copy_byte_code(Function *function, Byte *code, size_t code_length) {
  function->code = malloc(sizeof(Byte) * code_length);
  memcpy(function->code, code, code_length);
  function->code_length = code_length;
}