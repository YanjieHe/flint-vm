#include "test_jump.h"
#include "opcode.h"
#include "test.h"
#include "type.h"

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
                 DIV_I32, PUSH_I32_0, HALT};

  program = create_program_with_single_function(__FUNCTION__, code,
                                                sizeof(code) / sizeof(Byte));
  machine = create_machine(100);

  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].i32_v, 5);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_program(program);
  free_machine(machine);
}

void test_loop() {
  Program *program;
  Machine *machine;
  Function *entry;
  Byte code[] = {/* Push 0 to the stack */
                 PUSH_I64, 0,
                 /* Pop stack top to local variable 0 */
                 POP_LOCAL_I64, 0,
                 /* Push local variable 0 to stack */
                 PUSH_LOCAL_I64, 0,
                 /* Push constant 0 to the stack */
                 PUSH_I64_0,
                 /* Pop stack top to local variable 1 */
                 POP_LOCAL_I64, 1,
                 /* Push local variable 0 to stack */
                 PUSH_LOCAL_I64, 0,
                 /* Push constant 0 to the stack */
                 PUSH_I64_0,
                 /* Compare if greater than */
                 GT_I64,
                 /* Conditional jump if false */
                 JUMP_IF_FALSE, 0, 14,
                 /* Push local variable 1 to stack */
                 PUSH_LOCAL_I64, 1,
                 /* Push local variable 0 to stack */
                 PUSH_LOCAL_I64, 0,
                 /* Add top two stack values */
                 ADD_I64,
                 /* Pop stack top to local variable 1 */
                 POP_LOCAL_I64, 1,
                 /* Push local variable 0 to stack */
                 PUSH_LOCAL_I64, 0,
                 /* Push constant 1 to the stack */
                 PUSH_I64_1,
                 /* Subtract top two stack values */
                 SUB_I64,
                 /* Pop stack top to local variable 0 */
                 POP_LOCAL_I64, 0,
                 /* Unconditional jump */
                 JUMP, 255, 233,
                 /* Push local variable 1 to stack */
                 PUSH_LOCAL_I64, 1,
                 /* Push constant 0 to the stack */
                 PUSH_I32_0,
                 /* Halt the execution */
                 HALT};

  program = create_program_with_single_function(__FUNCTION__, code,
                                                sizeof(code) / sizeof(Byte));
  entry = program->entry;
  entry->locals = 2;
  entry->constant_pool_size = 1;
  entry->constant_pool = malloc(sizeof(Constant));
  entry->constant_pool[0].kind = CONSTANT_KIND_I64;
  entry->constant_pool[0].u.i64_v = 100;

  machine = create_machine(100);
  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].i64_v, 5050L);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_program(program);
  free_machine(machine);
}
