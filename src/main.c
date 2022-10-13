#include <stdio.h>

#include "test.h"
#include "test_arithmetic.h"


#define STACK_MAX_SIZE 10000

int main(int argc, char **argv) {
  total_tests = 0;
  passed_tests = 0;
  test_add();
  test_results_summary();
  /*
  ByteCodeLoader *loader;
  Program *program;
  Machine *machine;

  printf("sizeof(String) = %ld\n", sizeof(String));
  printf("sizeof(Value) = %ld\n", sizeof(Value));
  printf("sizeof(Array) = %ld\n", sizeof(Array));

  loader = create_byte_code_loader("../examples/byte_code.bin");
  if (loader) {
    program = read_byte_code_file(loader);
    if (program) {
      printf("program is successfully loaded.\n");
      printf("\n========== view program ==========\n");
      view_program(program);
      printf("========== run machine ==========\n");
      machine = create_machine(STACK_MAX_SIZE);
      machine->env.function = &program->modules[program->entry_module]
                                   .functions[program->entry_function];
      run_machine(machine);
    } else {
      printf("program is not successfully loaded.\n");
    }
  } else {
    printf("fail to load the byte code file\n");
  }
  */
  return 0;
}