#include "test_function.h"
#include "test.h"
#include "opcode.h"
#include "type.h"
#include "byte_code_loader.h"
#include "dlfcn.h"

void test_function_invocation() {
  Program *program;
  Machine *machine;
  Byte caller_code[] = {/* push first argument */
                        PUSH_I32_1BYTE, 25, CAST_I32_TO_F32,
                        /* push second argument */
                        PUSH_I32_1BYTE, 35, CAST_I32_TO_F32,
                        /* invoke function */
                        INVOKE_FUNCTION, 0};
  Byte callee_code[] = {PUSH_LOCAL_F32, 0, PUSH_LOCAL_F32, 1, MUL_F32, HALT};
  Function *caller;
  Function *callee;

  program = create_program("Program", 0, 0, 2, 0, 0);

  /* caller function */
  caller = &(program->functions[0]);
  copy_byte_code(caller, caller_code, sizeof(caller_code) / sizeof(Byte));
  caller->name = make_string("caller");
  caller->args_size = 0;
  caller->locals = 0;
  caller->stack = 0;
  caller->constant_pool_size = 1;
  caller->constant_pool =
      malloc(sizeof(Constant) * program->functions[0].constant_pool_size);

  /* callee function */
  callee = &(program->functions[1]);
  copy_byte_code(callee, callee_code, sizeof(callee_code) / sizeof(Byte));
  callee->name = make_string("callee");
  callee->args_size = 2;
  callee->locals = 0;
  callee->stack = 0;
  callee->constant_pool_size = 0;
  callee->constant_pool = NULL;

  /* update constant pool function reference */
  caller->constant_pool[0].u.func_v = callee;

  program->entry = caller;

  machine = create_machine(100);

  load_program(machine, program);
  run_machine(machine);
  ASSERT_EQUAL((i32)machine->stack[machine->sp].f32_v, 875);

  free_program(program);
  free_machine(machine);
}

void test_function_return_i32() {
  Program *program;
  Machine *machine;
  Byte caller_code[] = {/* push first argument */
                        PUSH_I32_1BYTE, 40,
                        /* push second argument */
                        PUSH_I32_1BYTE, 80,
                        /* invoke function */
                        INVOKE_FUNCTION, 0};
  Byte callee_code[] = {PUSH_LOCAL_I32, 0,         PUSH_LOCAL_I32, 1,
                        MUL_I32,        RETURN_I32};
  Function *caller;
  Function *callee;

  program = create_program("Program", 0, 0, 2, 0, 0);

  /* caller function */
  caller = &(program->functions[0]);
  copy_byte_code(caller, caller_code, sizeof(caller_code) / sizeof(Byte));
  caller->name = make_string("caller");
  caller->args_size = 0;
  caller->locals = 0;
  caller->stack = 0;
  caller->constant_pool_size = 1;
  caller->constant_pool =
      malloc(sizeof(Constant) * program->functions[0].constant_pool_size);

  /* callee function */
  callee = &(program->functions[1]);
  copy_byte_code(callee, callee_code, sizeof(callee_code) / sizeof(Byte));
  callee->name = make_string("callee");
  callee->args_size = 2;
  callee->locals = 0;
  callee->stack = 0;
  callee->constant_pool_size = 0;
  callee->constant_pool = NULL;

  /* update constant pool function reference */
  caller->constant_pool[0].u.func_v = callee;

  program->entry = caller;

  machine = create_machine(100);

  load_program(machine, program);
  run_machine(machine);
  ASSERT_EQUAL(machine->stack[machine->sp].i32_v, 3200);

  free_program(program);
  free_machine(machine);
}

void test_function_factorial() {
  Program *program;
  Machine *machine;
  Byte caller_code[] = {PUSH_I32_1BYTE, 10, CAST_I32_TO_I64,
                        /* invoke function */
                        INVOKE_FUNCTION, 0, HALT};
  Byte callee_code[] = {/* push N */
                        PUSH_LOCAL_I64, 0,
                        /* push 0L */
                        PUSH_I64_0,
                        /* N <= 0*/
                        LE_I64,
                        /* if true, jump */
                        JUMP_IF_TRUE, 0, 10,
                        /* push N */
                        PUSH_LOCAL_I64, 0,
                        /* push 1 */
                        PUSH_I64_1,
                        /* N - 1 */
                        SUB_I64,
                        /* call Factorial(N - 1) */
                        INVOKE_FUNCTION, 0,
                        /* push N */
                        PUSH_LOCAL_I64, 0,
                        /* N * Factorial(N - 1) */
                        MUL_I64,
                        /* return N * Factorial(N - 1) */
                        RETURN_I64,
                        /* push 1L */
                        PUSH_I64_1,
                        /* return 1L */
                        RETURN_I64};
  Function *caller;
  Function *callee;

  program = create_program("Program", 0, 0, 2, 0, 0);

  /* caller function */
  caller = &(program->functions[0]);
  copy_byte_code(caller, caller_code, sizeof(caller_code) / sizeof(Byte));
  caller->name = make_string("caller");
  caller->args_size = 0;
  caller->locals = 0;
  caller->stack = 0;
  caller->constant_pool_size = 1;
  caller->constant_pool = malloc(sizeof(Constant) * caller->constant_pool_size);

  /* callee function */
  callee = &(program->functions[1]);
  copy_byte_code(callee, callee_code, sizeof(callee_code) / sizeof(Byte));
  callee->name = make_string("callee");
  callee->args_size = 1;
  callee->locals = 0;
  callee->stack = 0;
  callee->constant_pool_size = 1;
  callee->constant_pool = malloc(sizeof(Constant) * callee->constant_pool_size);

  /* update constant pool function reference */
  caller->constant_pool[0].u.func_v = callee;
  callee->constant_pool[0].u.func_v = callee;

  program->entry = caller;

  machine = create_machine(100);

  load_program(machine, program);
  run_machine(machine);
  ASSERT_EQUAL(machine->stack[machine->sp].i64_v, 3628800L);

  free_program(program);
  free_machine(machine);
}

void test_function_native_function_call() {
  Program *program;
  Machine *machine;
  Function *function;
  NativeFunction *native_function;
  Byte code[] = {PUSH_STRING, 0, INVOKE_NATIVE_FUNCTION, 1, HALT};

  program = create_program_with_single_function(__FUNCTION__, code,
                                                sizeof(code) / sizeof(Byte));
  function = program->entry;
  function->constant_pool_size = 2;
  function->constant_pool =
      malloc(sizeof(Constant) * function->constant_pool_size);
  function->constant_pool[0].u.obj_v = malloc(sizeof(GCObject));
  function->constant_pool[0].u.obj_v->kind = GCOBJECT_KIND_STRING;
  function->constant_pool[0].u.obj_v->u.str_v = make_string("Hello, World!");
  function->constant_pool[1].u.native_func_v = malloc(sizeof(NativeFunction));
  native_function = function->constant_pool[1].u.native_func_v;
  native_function->args_size = 1;
  program->native_library_count = 1;
  program->native_library_handlers =
      malloc(sizeof(void *) * program->native_library_count);
  program->native_library_handlers[0] = dlopen(
      "../extensions/input-output/build/libflint-vm-input-output.so", RTLD_NOW);
  native_function->function_pointer =
      dlsym(program->native_library_handlers[0], "FLINT_VM_println");
  ASSERT_NOT_EQUAL(native_function->function_pointer, NULL);
  machine = create_machine(100);

  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_program(program);
  free_machine(machine);
}

void test_function_without_return_value() {
  /* TO DO */
}