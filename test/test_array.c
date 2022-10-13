#include "test_array.h"
#include "test.h"
#include "opcode.h"

void test_create_an_array() {
  Program *program;
  Machine *machine;
  Byte code[] = {PUSH_I32_1BYTE, 15, PUSH_ARRAY_I32};

  program =
      create_program_with_single_function(code, sizeof(code) / sizeof(Byte));

  machine = create_machine(100);

  load_program_on_machine(program, machine, 0, 0);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].u.arr_v->length, 15);

  /* TO DO: free the array */
  free_program_with_single_function(program);
  free_machine(machine);
}
