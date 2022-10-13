#include "test.h"

int total_tests;
int passed_tests;

void test_results_summary() {
  printf("total test cases: %d\n", total_tests);
  printf("passed test cases: %d\n", passed_tests);
}

Program *create_program_with_single_function(Byte *code, size_t code_length) {
  Program *program;

  program = malloc(sizeof(Program));

  program->file_name = NULL;
  program->module_count = 1;
  program->modules = malloc(sizeof(Module));
  program->structure_count = 0;
  program->structures = NULL;
  program->entry_module = 0;
  program->entry_function = 0;
  program->modules[0].function_count = 1;
  program->modules[0].functions = malloc(sizeof(Function));
  program->modules[0].functions[0].code = code;
  program->modules[0].functions[0].code_length = code_length;
  program->modules[0].functions[0].name = NULL;

  return program;
}

void free_program_with_single_function(Program *program) {
  free(program->modules[0].functions);
  free(program->modules);
  free(program);
}

void load_program_on_machine(Program *program, Machine *machine,
                             int main_module_id, int main_function_id) {
  machine->env.function =
      &(program->modules[main_module_id].functions[main_function_id]);
  machine->env.module = &(program->modules[main_module_id]);
}