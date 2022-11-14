#include "test_jump.h"
#include "test.h"
#include "opcode.h"
#include "type.h"
#include "byte_code_loader.h"
#include "byte_code_printer.h"

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
                 DIV_I32, HALT};

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
  ByteCodeLoader *loader;

  loader = create_byte_code_loader("byte_code/loop");
  ASSERT_NOT_EQUAL(loader, NULL);

  program = read_byte_code_file(loader);
  show_errors(loader->error_messages);
  ASSERT_NOT_EQUAL(program, NULL);
  ASSERT_EQUAL(loader->error_messages, NULL);

  machine = create_machine(100);
  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].i64_v, 5050L);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_byte_code_loader(loader);
  free_program(program);
  free_machine(machine);
}