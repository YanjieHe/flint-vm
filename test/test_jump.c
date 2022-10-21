#include "test_jump.h"
#include "test.h"
#include "opcode.h"
#include "type.h"

void test_jump() {
  Program *program;
  Machine *machine;
  Byte code[] = {PUSH_I32_1BYTE, 10, PUSH_I32_2BYTES, 2, 5, ADD_I32};

  program =
      create_program_with_single_function(__FUNCTION__, code, sizeof(code) / sizeof(Byte));
  machine = create_machine(100);

  load_program_on_machine(program, machine, 0, 0);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].i32_v, 527);
  ASSERT_EQUAL(machine->machine_status, MACHINE_COMPLETED);

  free_program(program);
  free_machine(machine);
}