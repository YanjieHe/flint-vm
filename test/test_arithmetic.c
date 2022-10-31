#include "test_arithmetic.h"
#include "value.h"
#include "opcode.h"
#include "machine.h"
#include <stdio.h>
#include "test.h"

void test_add() {
  Program *program;
  Machine *machine;
  Byte code[] = {PUSH_I32_1BYTE, 10, PUSH_I32_2BYTES, 2, 5, ADD_I32, HALT};

  program = create_program_with_single_function(__FUNCTION__, code,
                                                sizeof(code) / sizeof(Byte));
  machine = create_machine(100);

  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].i32_v, 527);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_program(program);
  free_machine(machine);
}