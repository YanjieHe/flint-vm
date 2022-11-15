#include <stdio.h>

#include "test.h"
#include "test_arithmetic.h"
#include "test_array.h"
#include "test_function.h"
#include "test_jump.h"
#include "test_structure.h"
#include "test_global_variable.h"


#define STACK_MAX_SIZE 10000

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

  test_jump();
  test_loop();

  test_structure();
  test_structure_get_field_value();
  
  test_global_variable();

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