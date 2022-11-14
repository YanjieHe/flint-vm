#include "test_arithmetic.h"
#include "value.h"
#include "opcode.h"
#include "machine.h"
#include <stdio.h>
#include "test.h"
#include "byte_code_loader.h"

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

void test_subtract() {
  Program *program;
  Machine *machine;
  ByteCodeLoader *loader;

  loader = create_byte_code_loader("byte_code/subtract");
  ASSERT_NOT_EQUAL(loader, NULL);

  program = read_byte_code_file(loader);
  show_errors(loader->error_messages);
  ASSERT_NOT_EQUAL(program, NULL);
  ASSERT_EQUAL(loader->error_messages, NULL);

  machine = create_machine(100);
  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].i32_v, 10464);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_byte_code_loader(loader);
  free_program(program);
  free_machine(machine);
}