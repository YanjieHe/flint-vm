#include "test_native_api.h"
#include "opcode.h"
#include "test.h"

static void prepare_native_frame(Machine *machine, i32 fp, i32 sp) {
  i32 i;

  machine->fp = fp;
  machine->sp = sp;
  memset(machine->is_gc_object, 0,
         sizeof(u8) * (size_t)machine->stack_max_size);
  if (fp > 0) {
    machine->is_gc_object[fp - 1] = TRUE;
  }
  for (i = fp; i <= sp; i++) {
    machine->is_gc_object[i] = TRUE;
  }
}

static int native_return_i32(Machine *machine) {
  FLINT_RETURN_I32(machine, 42);
}

static int native_return_i64(Machine *machine) {
  FLINT_RETURN_I64(machine, 4200);
}

static int native_return_f32(Machine *machine) {
  FLINT_RETURN_F32(machine, 3.5f);
}

static int native_return_f64(Machine *machine) {
  FLINT_RETURN_F64(machine, 7.25);
}

static int native_return_bool(Machine *machine) {
  FLINT_RETURN_BOOL(machine, TRUE);
}

static int native_return_char(Machine *machine) {
  FLINT_RETURN_CHAR(machine, 0x4E2D);
}

static int native_return_first_object(Machine *machine) {
  GCObject *object;

  object = FLINT_GET_OBJECT_ARG(machine, 0);
  FLINT_RETURN_OBJECT(machine, object);
}

static int native_return_void(Machine *machine) {
  FLINT_RETURN_VOID(machine);
}

static int native_add(Machine *machine) {
  i32 left;
  i32 right;

  left = FLINT_GET_I32_ARG(machine, 0);
  right = FLINT_GET_I32_ARG(machine, 1);
  FLINT_RETURN_I32(machine, left + right);
}

static int native_error(Machine *machine) {
  (void)machine;
  return NATIVE_FUNCTION_ERROR;
}

static void set_native_function_pointer(NativeFunction *native_function,
                                        int (*function)(Machine *)) {
  union {
    void *pointer;
    int (*function)(Machine *);
  } conversion;

  conversion.function = function;
  native_function->function_pointer = conversion.pointer;
}

void test_native_primitive_argument_macros() {
  Machine *machine;

  machine = create_machine(16);
  machine->fp = 3;
  machine->stack[3].i32_v = 42;
  machine->stack[4].i64_v = 1000;
  machine->stack[5].f32_v = 3.5f;
  machine->stack[6].f64_v = 7.25;
  machine->stack[7].i32_v = TRUE;
  machine->stack[8].i32_v = 0x4E2D;

  ASSERT_EQUAL(FLINT_GET_I32_ARG(machine, 0), 42);
  ASSERT_EQUAL(FLINT_GET_I64_ARG(machine, 1), 1000);
  ASSERT_EQUAL(FLINT_GET_F32_ARG(machine, 2), 3.5f);
  ASSERT_EQUAL(FLINT_GET_F64_ARG(machine, 3), 7.25);
  ASSERT_EQUAL(FLINT_GET_BOOL_ARG(machine, 4), TRUE);
  ASSERT_EQUAL(FLINT_GET_CHAR_ARG(machine, 5), 0x4E2D);

  machine->stack[7].i32_v = FALSE;
  ASSERT_EQUAL(FLINT_GET_BOOL_ARG(machine, 4), FALSE);

  free_machine(machine);
}

void test_native_object_argument_macros() {
  Machine *machine;
  String string;
  u32 string_characters[] = {'t', 'e', 's', 't'};
  Array array;
  Structure structure;
  GCObject string_object;
  GCObject array_object;
  GCObject structure_object;

  machine = create_machine(16);
  machine->fp = 2;

  string.length = 4;
  string.characters = string_characters;
  string_object.kind = GCOBJECT_KIND_STRING;
  string_object.u.str_v = &string;

  array.length = 0;
  array_object.kind = GCOBJECT_KIND_I32_ARRAY;
  array_object.u.arr_v = &array;

  structure.meta_data = NULL;
  structure.values = NULL;
  structure_object.kind = GCOBJECT_KIND_OBJECT;
  structure_object.u.struct_v = &structure;

  machine->stack[2].obj_v = &string_object;
  machine->stack[3].obj_v = &array_object;
  machine->stack[4].obj_v = &structure_object;

  ASSERT_EQUAL(FLINT_GET_OBJECT_ARG(machine, 0), &string_object);
  ASSERT_EQUAL(FLINT_GET_STRING_ARG(machine, 0), &string);
  ASSERT_EQUAL(FLINT_GET_ARRAY_ARG(machine, 1), &array);
  ASSERT_EQUAL(FLINT_GET_STRUCT_ARG(machine, 2), &structure);

  free_machine(machine);
}

void test_native_primitive_return_macros() {
  Machine *machine;
  int result;

  machine = create_machine(16);

  prepare_native_frame(machine, 2, 4);
  result = native_return_i32(machine);
  ASSERT_EQUAL(result, NATIVE_FUNCTION_SUCCESS);
  ASSERT_EQUAL(machine->sp, 2);
  ASSERT_EQUAL(machine->stack[2].i32_v, 42);
  ASSERT_EQUAL(machine->is_gc_object[1], TRUE);
  ASSERT_EQUAL(machine->is_gc_object[2], FALSE);
  ASSERT_EQUAL(machine->is_gc_object[3], FALSE);
  ASSERT_EQUAL(machine->is_gc_object[4], FALSE);

  prepare_native_frame(machine, 2, 4);
  result = native_return_i64(machine);
  ASSERT_EQUAL(result, NATIVE_FUNCTION_SUCCESS);
  ASSERT_EQUAL(machine->sp, 2);
  ASSERT_EQUAL(machine->stack[2].i64_v, 4200);
  ASSERT_EQUAL(machine->is_gc_object[2], FALSE);

  prepare_native_frame(machine, 2, 4);
  result = native_return_f32(machine);
  ASSERT_EQUAL(result, NATIVE_FUNCTION_SUCCESS);
  ASSERT_EQUAL(machine->sp, 2);
  ASSERT_EQUAL(machine->stack[2].f32_v, 3.5f);
  ASSERT_EQUAL(machine->is_gc_object[2], FALSE);

  prepare_native_frame(machine, 2, 4);
  result = native_return_f64(machine);
  ASSERT_EQUAL(result, NATIVE_FUNCTION_SUCCESS);
  ASSERT_EQUAL(machine->sp, 2);
  ASSERT_EQUAL(machine->stack[2].f64_v, 7.25);
  ASSERT_EQUAL(machine->is_gc_object[2], FALSE);

  prepare_native_frame(machine, 2, 4);
  result = native_return_bool(machine);
  ASSERT_EQUAL(result, NATIVE_FUNCTION_SUCCESS);
  ASSERT_EQUAL(machine->sp, 2);
  ASSERT_EQUAL(machine->stack[2].i32_v, TRUE);
  ASSERT_EQUAL(machine->is_gc_object[2], FALSE);

  prepare_native_frame(machine, 2, 4);
  result = native_return_char(machine);
  ASSERT_EQUAL(result, NATIVE_FUNCTION_SUCCESS);
  ASSERT_EQUAL(machine->sp, 2);
  ASSERT_EQUAL(machine->stack[2].i32_v, 0x4E2D);
  ASSERT_EQUAL(machine->is_gc_object[2], FALSE);

  free_machine(machine);
}

void test_native_object_return_macro() {
  Machine *machine;
  GCObject object;
  int result;

  machine = create_machine(16);
  object.kind = GCOBJECT_KIND_OBJECT;

  prepare_native_frame(machine, 2, 4);
  machine->stack[2].obj_v = &object;
  result = native_return_first_object(machine);

  ASSERT_EQUAL(result, NATIVE_FUNCTION_SUCCESS);
  ASSERT_EQUAL(machine->sp, 2);
  ASSERT_EQUAL(machine->stack[2].obj_v, &object);
  ASSERT_EQUAL(machine->is_gc_object[1], TRUE);
  ASSERT_EQUAL(machine->is_gc_object[2], TRUE);
  ASSERT_EQUAL(machine->is_gc_object[3], FALSE);
  ASSERT_EQUAL(machine->is_gc_object[4], FALSE);

  free_machine(machine);
}

void test_native_void_and_zero_argument_returns() {
  Machine *machine;
  int result;

  machine = create_machine(16);

  prepare_native_frame(machine, 2, 4);
  result = native_return_void(machine);
  ASSERT_EQUAL(result, NATIVE_FUNCTION_SUCCESS);
  ASSERT_EQUAL(machine->sp, 2);
  ASSERT_EQUAL(machine->is_gc_object[1], TRUE);
  ASSERT_EQUAL(machine->is_gc_object[2], FALSE);
  ASSERT_EQUAL(machine->is_gc_object[3], FALSE);
  ASSERT_EQUAL(machine->is_gc_object[4], FALSE);

  prepare_native_frame(machine, 3, 2);
  machine->is_gc_object[3] = TRUE;
  result = native_return_i32(machine);
  ASSERT_EQUAL(result, NATIVE_FUNCTION_SUCCESS);
  ASSERT_EQUAL(machine->sp, 3);
  ASSERT_EQUAL(machine->stack[3].i32_v, 42);
  ASSERT_EQUAL(machine->is_gc_object[3], FALSE);

  prepare_native_frame(machine, 3, 2);
  machine->is_gc_object[3] = TRUE;
  result = native_return_void(machine);
  ASSERT_EQUAL(result, NATIVE_FUNCTION_SUCCESS);
  ASSERT_EQUAL(machine->sp, 3);
  ASSERT_EQUAL(machine->is_gc_object[3], FALSE);

  free_machine(machine);
}

void test_native_api_through_invoke_instruction() {
  Program *program;
  Function *entry;
  NativeFunction *native_function;
  Machine *machine;
  i32 exit_code;
  Byte code[] = {PUSH_I32_1BYTE, 20, PUSH_I32_1BYTE, 22,
                 INVOKE_NATIVE_FUNCTION, 0, HALT};

  program = create_program("Program", 0, 0, 1, 0, 1, 0, 0);
  entry = program->entry;
  copy_byte_code(entry, code, sizeof(code) / sizeof(Byte));
  entry->constant_pool_size = 1;
  entry->constant_pool = malloc(sizeof(Constant));

  native_function = &(program->native_functions[0]);
  native_function->args_size = 2;
  set_native_function_pointer(native_function, native_add);
  entry->constant_pool[0].u.native_func_v = native_function;

  machine = create_machine(16);
  load_program(machine, program);
  exit_code = run_machine(machine);

  ASSERT_EQUAL(exit_code, 42);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);
  ASSERT_EQUAL(machine->sp, -1);

  free_program(program);
  free_machine(machine);
}

void test_native_function_error_sets_machine_status() {
  Program *program;
  Function *entry;
  NativeFunction *native_function;
  Machine *machine;
  Byte code[] = {INVOKE_NATIVE_FUNCTION, 0};

  program = create_program("Program", 0, 0, 1, 0, 1, 0, 0);
  entry = program->entry;
  copy_byte_code(entry, code, sizeof(code) / sizeof(Byte));
  entry->constant_pool_size = 1;
  entry->constant_pool = malloc(sizeof(Constant));

  native_function = &(program->native_functions[0]);
  native_function->args_size = 0;
  set_native_function_pointer(native_function, native_error);
  entry->constant_pool[0].u.native_func_v = native_function;

  machine = create_machine(16);
  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->machine_status, RUNTIME_ERROR_NATIVE_FUNCTION_ERROR);

  free_program(program);
  free_machine(machine);
}
