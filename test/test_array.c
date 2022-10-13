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

  ASSERT_EQUAL(machine->stack[machine->sp].obj_v->u.arr_v->length, 15);
  ASSERT_EQUAL(machine->stack[machine->sp].obj_v->kind,
               GCOBJECT_KIND_I32_ARRAY);
  ASSERT_EQUAL(machine->is_gc_object[machine->sp], 1);
  ASSERT_EQUAL(machine->machine_status, MACHINE_COMPLETED);

  free_program_with_single_function(program);
  free_machine(machine);
}

void test_create_an_illegal_array() {
  Program *program;
  Machine *machine;
  Byte code[] = {PUSH_I32_1BYTE, 5, MINUS_I32, PUSH_ARRAY_F32};

  program =
      create_program_with_single_function(code, sizeof(code) / sizeof(Byte));

  machine = create_machine(100);

  load_program_on_machine(program, machine, 0, 0);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].i32_v, -5);
  ASSERT_EQUAL(machine->machine_status,
               RUNTIME_ERROR_ARRAY_LENGTH_LESS_THAN_ZERO);
  ASSERT_EQUAL(machine->heap, NULL);

  free_program_with_single_function(program);
  free_machine(machine);
}