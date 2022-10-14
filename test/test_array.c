#include "test_array.h"
#include "test.h"
#include "opcode.h"
#include "type.h"

void test_create_an_array() {
  Program *program;
  Machine *machine;
  Byte code[] = {PUSH_I32_1BYTE, 15, NEW_ARRAY, TYPE_I32};

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
  Byte code[] = {PUSH_I32_1BYTE, 5, MINUS_I32, NEW_ARRAY, TYPE_F32};

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

void test_access_array() {
  Program *program;
  Machine *machine;
  GCObject *array_obj;
  Byte code[] = {
      /* create an integer 64 array */
      PUSH_I32_1BYTE, 5, NEW_ARRAY, TYPE_I64,
      /* store it as a local variable */
      POP_LOCAL_OBJECT, 0,
      /* get the local variable */
      PUSH_LOCAL_OBJECT, 0,
      /* push the array offset */
      PUSH_I32_1BYTE, 3,
      /* push the value */
      PUSH_I32_2BYTES, 11, 89, CAST_I32_TO_I64,
      /* store the value in the array at the specific location */
      POP_ARRAY_I64,
      /* get the local variable */
      PUSH_LOCAL_OBJECT, 0,
      /* push the array offset */
      PUSH_I32_1BYTE, 3,
      /* push array element */
      PUSH_ARRAY_I64,
  };

  program =
      create_program_with_single_function(code, sizeof(code) / sizeof(Byte));
  machine = create_machine(100);

  load_program_on_machine(program, machine, 0, 0);

  /* create space for the array variable */
  machine->sp++;

  run_machine(machine);

  ASSERT_EQUAL(machine->machine_status, MACHINE_COMPLETED);
  ASSERT_EQUAL(machine->stack[machine->sp].i64_v, 2905);

  array_obj = machine->stack[0].obj_v;
  ASSERT_EQUAL(array_obj->kind, GCOBJECT_KIND_I64_ARRAY);
  ASSERT_EQUAL(array_obj->u.arr_v->length, 5);
  ASSERT_EQUAL(array_obj->u.arr_v->u.i64_array[3], 2905);

  free_program_with_single_function(program);
  free_machine(machine);
}