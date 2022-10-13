#include "test_arithmetic.h"
#include "value.h"
#include "opcode.h"
#include "machine.h"
#include <stdio.h>
#include "test.h"

void test_add() {
  Program *program;
  Machine *machine;
  Byte code[] = {PUSH_I32_1BYTE, 10, PUSH_I32_2BYTES, 2, 5, ADD_I32};

  program =
      create_program_with_single_function(code, sizeof(code) / sizeof(Byte));

  machine = create_machine(100);

  load_program_on_machine(program, machine, 0, 0);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].u.i32_v, 527);

  free_program_with_single_function(program);
  free_machine(machine);
}