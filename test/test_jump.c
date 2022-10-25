#include "test_jump.h"
#include "test.h"
#include "opcode.h"
#include "type.h"
#include "byte_code_loader.h"

void test_jump() {
  Program *program;
  Machine *machine;
  Byte code[] = {/* push a number */
                 PUSH_I32_1BYTE, 10,
                 /* jump */
                 JUMP, 0, 3,
                 /* skipped code */
                 PUSH_I32_2BYTES, 4, 5,
                 /* push another number */
                 PUSH_I32_1BYTE, 2,
                 /* divide */
                 DIV_I32};

  program = create_program_with_single_function(__FUNCTION__, code,
                                                sizeof(code) / sizeof(Byte));
  machine = create_machine(100);

  load_program_on_machine(program, machine, 0, 0);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].i32_v, 5);
  ASSERT_EQUAL(machine->machine_status, MACHINE_COMPLETED);

  free_program(program);
  free_machine(machine);
}