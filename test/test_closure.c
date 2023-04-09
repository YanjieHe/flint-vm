#include "test_closure.h"
#include "test.h"
#include "opcode.h"
#include "type.h"

void test_closure_invocation() {
  Program *program;
  Machine *machine;

  Byte closure_creation_code[] = {/* env */
                                  NEW, 0, DUPLICATE,
                                  /* env.a = 7L */
                                  PUSH_I32_1BYTE, 7, CAST_I32_TO_I64,
                                  POP_FIELD_I64, 0,
                                  /* createa a closure */
                                  NEW_CLOSURE, 1,
                                  /* return the closure */
                                  RETURN_OBJECT};

  /* lambda (x) => x % a */
  Byte closure_code[] = {/* push local variable x */
                         PUSH_LOCAL_I64, 0,
                         /* push local variable env */
                         PUSH_LOCAL_OBJECT, 1,
                         /* env.a */
                         PUSH_FIELD_I64, 0,
                         /* x % a */
                         MOD_I64,
                         /* return (x % a) */
                         RETURN_I64};

  Byte entry_code[] = {/* push 39L */
                       PUSH_I32_1BYTE, 39, CAST_I32_TO_I64,
                       /* lambda (x) => x % a */
                       INVOKE_FUNCTION, 0,
                       /* call the closure */
                       INVOKE_CLOSURE, 1, PUSH_I32_0, HALT};

  Function *create_closure;
  Function *closure;
  Function *entry;

  StructureMetaData *captured_values;

  program = create_program("Program", 0, 1, 3, 0, 0, 0);

  /* function create_closure() */
  create_closure = &(program->functions[0]);
  copy_byte_code(create_closure, closure_creation_code,
                 sizeof(closure_creation_code) / sizeof(Byte));
  create_closure->name = make_string("create_closure");
  create_closure->args_size = 0;
  create_closure->locals = 0;
  create_closure->stack = 0;
  create_closure->constant_pool_size = 2;
  create_closure->constant_pool =
      malloc(sizeof(Constant) * create_closure->constant_pool_size);

  /* function closure(x) */
  closure = &(program->functions[1]);
  copy_byte_code(closure, closure_code, sizeof(closure_code) / sizeof(Byte));
  closure->name = make_string("closure");
  closure->args_size = 2;
  closure->locals = 0;
  closure->stack = 0;
  closure->constant_pool_size = 0;
  closure->constant_pool = NULL;

  /* entry function */
  entry = &(program->functions[2]);
  copy_byte_code(entry, entry_code, sizeof(entry_code) / sizeof(Byte));
  entry->name = make_string("entry");
  entry->args_size = 0;
  entry->locals = 0;
  entry->stack = 0;
  entry->constant_pool_size = 2;
  entry->constant_pool = malloc(sizeof(Constant) * entry->constant_pool_size);

  /* closure captured values */
  captured_values = &(program->structures_meta_data[0]);
  captured_values->n_values = 1;
  captured_values->name = NULL;
  captured_values->field_names =
      malloc(sizeof(String *) * captured_values->n_values);
  captured_values->field_names[0] = make_string("a");

  /* update constant pool function reference */
  create_closure->constant_pool[0].kind = CONSTANT_KIND_STRUCTURE_META_DATA;
  create_closure->constant_pool[0].u.struct_meta_data = captured_values;
  create_closure->constant_pool[1].kind = CONSTANT_KIND_FUNCTION;
  create_closure->constant_pool[1].u.func_v = closure;

  entry->constant_pool[0].kind = CONSTANT_KIND_FUNCTION;
  entry->constant_pool[0].u.func_v = create_closure;
  entry->constant_pool[1].kind = CONSTANT_KIND_FUNCTION;
  entry->constant_pool[1].u.func_v = NULL;

  program->entry = entry;

  machine = create_machine(100);

  load_program(machine, program);
  run_machine(machine);
  ASSERT_EQUAL(machine->stack[machine->sp].i64_v, 39 % 7);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_program(program);
  free_machine(machine);
}