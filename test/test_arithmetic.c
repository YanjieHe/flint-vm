#include "test_arithmetic.h"
#include "machine.h"
#include "opcode.h"
#include "test.h"
#include "value.h"
#include <stdio.h>

void test_add() {
  Program *program;
  Machine *machine;
  Byte code[] = {PUSH_I32_1BYTE, 10,  PUSH_I32_2BYTES, 2, 5, ADD_I32,
                 PUSH_I32_0,     HALT};

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

void test_subtract() {
  Program *program;
  Machine *machine;
  Byte code[] = {PUSH_I32_2BYTES,
                 41,
                 23, /* PUSH 10519 */
                 PUSH_I32_1BYTE,
                 55, /* PUSH 55 */
                 SUB_I32,
                 PUSH_I32_0, /* PUSH 0 */
                 HALT};

  program = create_program_with_single_function(__FUNCTION__, code,
                                                sizeof(code) / sizeof(Byte));

  machine = create_machine(100);
  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].i32_v, 10464);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_program(program);
  free_machine(machine);
}
