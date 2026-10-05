#include "test_string.h"
#include "opcode.h"
#include "test.h"

static GCObject *add_string_constant(Function *function, i32 index,
                                     const char *utf8) {
  GCObject *object;

  object = wrap_string_into_gc_object(make_string(utf8));
  function->constant_pool[index].kind = CONSTANT_KIND_STRING;
  function->constant_pool[index].u.obj_v = object;

  return object;
}

static void free_string_constants(GCObject **objects, i32 count) {
  i32 i;

  for (i = 0; i < count; i++) {
    free_gc_object(objects[i]);
  }
}

static i32 run_string_comparison(Byte opcode, const char *left,
                                 const char *right, u8 *result_gc_flag,
                                 u8 *discarded_gc_flag) {
  Byte code[] = {PUSH_STRING, 0, PUSH_STRING, 1, opcode, PUSH_I32_0, HALT};
  Program *program;
  Function *entry;
  Machine *machine;
  GCObject *objects[2];
  i32 result;

  program = create_program_with_single_function(__FUNCTION__, code,
                                                sizeof(code) / sizeof(Byte));
  entry = program->entry;
  entry->constant_pool_size = 2;
  entry->constant_pool = malloc(sizeof(Constant) * 2);
  objects[0] = add_string_constant(entry, 0, left);
  objects[1] = add_string_constant(entry, 1, right);

  machine = create_machine(100);
  load_program(machine, program);
  run_machine(machine);

  result = machine->stack[machine->sp].i32_v;
  *result_gc_flag = machine->is_gc_object[machine->sp];
  *discarded_gc_flag = machine->is_gc_object[machine->sp + 1];

  free_program(program);
  free_machine(machine);
  free_string_constants(objects, 2);

  return result;
}

static void assert_string_comparison(Byte opcode, const char *left,
                                     const char *right, i32 expected) {
  u8 result_gc_flag;
  u8 discarded_gc_flag;

  ASSERT_EQUAL(run_string_comparison(opcode, left, right, &result_gc_flag,
                                     &discarded_gc_flag),
               expected);
  ASSERT_EQUAL(result_gc_flag, FALSE);
  ASSERT_EQUAL(discarded_gc_flag, FALSE);
}

void test_push_string_char() {
  Byte code[] = {PUSH_STRING,
                 0,
                 PUSH_I32_0,
                 PUSH_STRING_CHAR,
                 PUSH_STRING,
                 0,
                 PUSH_I32_1,
                 PUSH_STRING_CHAR,
                 PUSH_STRING,
                 0,
                 PUSH_I32_1BYTE,
                 2,
                 PUSH_STRING_CHAR,
                 PUSH_I32_0,
                 HALT};
  Program *program;
  Function *entry;
  Machine *machine;
  GCObject *string_object;

  program = create_program_with_single_function(__FUNCTION__, code,
                                                sizeof(code) / sizeof(Byte));
  entry = program->entry;
  entry->constant_pool_size = 1;
  entry->constant_pool = malloc(sizeof(Constant));
  string_object =
      add_string_constant(entry, 0, "A\xE4\xB8\xAD\xF0\x9F\x98\x80");

  machine = create_machine(100);
  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);
  ASSERT_EQUAL(machine->sp, 2);
  ASSERT_EQUAL(machine->stack[0].i32_v, 0x41);
  ASSERT_EQUAL(machine->stack[1].i32_v, 0x4E2D);
  ASSERT_EQUAL(machine->stack[2].i32_v, 0x1F600);
  ASSERT_EQUAL(machine->is_gc_object[0], FALSE);
  ASSERT_EQUAL(machine->is_gc_object[1], FALSE);
  ASSERT_EQUAL(machine->is_gc_object[2], FALSE);

  free_program(program);
  free_machine(machine);
  free_gc_object(string_object);
}

static void assert_string_index_error(Byte *code, size_t code_length) {
  Program *program;
  Function *entry;
  Machine *machine;
  GCObject *string_object;

  program =
      create_program_with_single_function(__FUNCTION__, code, code_length);
  entry = program->entry;
  entry->constant_pool_size = 1;
  entry->constant_pool = malloc(sizeof(Constant));
  string_object = add_string_constant(entry, 0, "abc");

  machine = create_machine(100);
  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->machine_status,
               RUNTIME_ERROR_STRING_INDEX_OUT_OF_RANGE);
  ASSERT_EQUAL(machine->sp, 0);
  ASSERT_EQUAL(machine->stack[machine->sp].obj_v, string_object);
  ASSERT_EQUAL(machine->is_gc_object[machine->sp], TRUE);

  free_program(program);
  free_machine(machine);
  free_gc_object(string_object);
}

void test_push_string_char_out_of_range() {
  Byte negative_index_code[] = {PUSH_STRING, 0, PUSH_I32_1, MINUS_I32,
                                PUSH_STRING_CHAR};
  Byte upper_bound_code[] = {PUSH_STRING, 0, PUSH_I32_1BYTE, 3,
                             PUSH_STRING_CHAR};

  assert_string_index_error(negative_index_code,
                            sizeof(negative_index_code) /
                                sizeof(negative_index_code[0]));
  assert_string_index_error(upper_bound_code, sizeof(upper_bound_code) /
                                                  sizeof(upper_bound_code[0]));
}

void test_string_length() {
  Byte code[] = {PUSH_STRING,   0,          STRING_LENGTH, PUSH_STRING, 1,
                 STRING_LENGTH, PUSH_I32_0, HALT};
  Program *program;
  Function *entry;
  Machine *machine;
  GCObject *objects[2];

  program = create_program_with_single_function(__FUNCTION__, code,
                                                sizeof(code) / sizeof(Byte));
  entry = program->entry;
  entry->constant_pool_size = 2;
  entry->constant_pool = malloc(sizeof(Constant) * 2);
  objects[0] = add_string_constant(entry, 0, "");
  objects[1] = add_string_constant(entry, 1, "A\xE4\xB8\xAD\xF0\x9F\x98\x80");

  machine = create_machine(100);
  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);
  ASSERT_EQUAL(machine->sp, 1);
  ASSERT_EQUAL(machine->stack[0].i32_v, 0);
  ASSERT_EQUAL(machine->stack[1].i32_v, 3);
  ASSERT_EQUAL(machine->is_gc_object[0], FALSE);
  ASSERT_EQUAL(machine->is_gc_object[1], FALSE);

  free_program(program);
  free_machine(machine);
  free_string_constants(objects, 2);
}

void test_string_comparisons() {
  assert_string_comparison(EQ_STRING, "same", "same", TRUE);
  assert_string_comparison(EQ_STRING, "abc", "abd", FALSE);
  assert_string_comparison(NE_STRING, "abc", "abd", TRUE);
  assert_string_comparison(NE_STRING, "same", "same", FALSE);
  assert_string_comparison(LT_STRING, "abc", "abcd", TRUE);
  assert_string_comparison(LT_STRING, "abd", "abc", FALSE);
  assert_string_comparison(LE_STRING, "same", "same", TRUE);
  assert_string_comparison(LE_STRING, "abd", "abc", FALSE);
  assert_string_comparison(GT_STRING, "abd", "abc", TRUE);
  assert_string_comparison(GT_STRING, "abc", "abd", FALSE);
  assert_string_comparison(GE_STRING, "same", "same", TRUE);
  assert_string_comparison(GE_STRING, "abc", "abd", FALSE);
  assert_string_comparison(LT_STRING, "\xE4\xB8\xAD", "\xF0\x9F\x98\x80", TRUE);
}
