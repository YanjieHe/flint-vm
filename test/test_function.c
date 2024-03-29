#include "test_function.h"
#include "test.h"
#include "opcode.h"
#include "type.h"
#include "byte_code_loader.h"
#include "load_library.h"

#define RUN_EVEN_ODD_PROGRAM_WITH_INPUT(INTEGER)                               \
  entry->constant_pool[0].u.i32_v = (INTEGER);                                 \
  machine = create_machine(100);                                               \
  load_program(machine, program);                                              \
  run_machine(machine);                                                        \
  ASSERT_EQUAL(machine->stack[machine->sp].i32_v, (INTEGER) % 2 == 0);         \
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);                      \
  free_machine(machine);

void test_function_invocation() {
  Program *program;
  Machine *machine;
  Byte caller_code[] = {/* push first argument */
                        PUSH_I32_1BYTE, 25, CAST_I32_TO_F32,
                        /* push second argument */
                        PUSH_I32_1BYTE, 35, CAST_I32_TO_F32,
                        /* invoke function */
                        INVOKE_FUNCTION, 0};
  Byte callee_code[] = {PUSH_LOCAL_F32, 0, PUSH_LOCAL_F32, 1, MUL_F32, PUSH_I32_0, HALT};
  Function *caller;
  Function *callee;

  program = create_program("Program", 0, 0, 2, 0, 0, 0);

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
                        INVOKE_FUNCTION, 0, PUSH_I32_0, HALT};
  Byte callee_code[] = {PUSH_LOCAL_I32, 0,         PUSH_LOCAL_I32, 1,
                        MUL_I32,        RETURN_I32};
  Function *caller;
  Function *callee;

  program = create_program("Program", 0, 0, 2, 0, 0, 0);

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
                        INVOKE_FUNCTION, 0, PUSH_I32_0, HALT};
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

  program = create_program("Program", 0, 0, 2, 0, 0, 0);

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
  Byte code[] = {PUSH_STRING, 0, INVOKE_NATIVE_FUNCTION, 1, PUSH_I32_0, HALT};

  program = create_program("Program", 0, 0, 1, 1, 1, 0);

  function = program->entry;
  function->constant_pool_size = 2;
  function->constant_pool =
      malloc(sizeof(Constant) * function->constant_pool_size);
  function->constant_pool[0].u.obj_v =
      wrap_string_into_gc_object(make_string("Hello, World!"));
  copy_byte_code(function, code, sizeof(code) / sizeof(Byte));

  native_function = &(program->native_functions[0]);
  native_function->args_size = 1;
  program->native_libraries[0].library_pointer = open_dynamic_library(
      "../extensions/input-output/build/libflint-vm-input-output.so");
  native_function->function_pointer =
      load_function_from_dynamic_library(program->native_libraries[0].library_pointer, "FLINT_VM_println");
  function->constant_pool[1].u.native_func_v = native_function;
  ASSERT_NOT_EQUAL(native_function->function_pointer, NULL);
  machine = create_machine(100);

  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_program(program);
  free_machine(machine);
}

void test_function_tail_call() {
  Program *program;
  Machine *machine;

  Byte caller_code[] = {/* push the first value in the constant pool */
                        PUSH_F32, 0,
                        /* push the second value in the constant pool */
                        PUSH_F32, 1,
                        /* tail call */
                        TAIL_CALL, 2};
  Byte callee_code[] = {/* push local variable a */
                        PUSH_LOCAL_F32, 0,
                        /* push local variable b */
                        PUSH_LOCAL_F32, 1,
                        /* a * b */
                        MUL_F32,
                        /* return (a * b) */
                        RETURN_F32};
  Byte entry_code[] = {INVOKE_FUNCTION, 0, PUSH_I32_0, HALT};

  Function *caller;
  Function *callee;
  Function *entry;

  program = create_program("Program", 0, 0, 3, 0, 0, 0);

  /* caller function */
  caller = &(program->functions[0]);
  copy_byte_code(caller, caller_code, sizeof(caller_code) / sizeof(Byte));
  caller->name = make_string("caller");
  caller->args_size = 0;
  caller->locals = 0;
  caller->stack = 0;
  caller->constant_pool_size = 3;
  caller->constant_pool = malloc(sizeof(Constant) * caller->constant_pool_size);

  /* callee function */
  callee = &(program->functions[1]);
  copy_byte_code(callee, callee_code, sizeof(callee_code) / sizeof(Byte));
  callee->name = make_string("callee");
  callee->args_size = 2;
  callee->locals = 0;
  callee->stack = 0;
  callee->constant_pool_size = 0;
  callee->constant_pool = malloc(sizeof(Constant) * callee->constant_pool_size);

  /* entry function */
  entry = &(program->functions[2]);
  copy_byte_code(entry, entry_code, sizeof(entry_code) / sizeof(Byte));
  entry->name = make_string("entry");
  entry->args_size = 0;
  entry->locals = 0;
  entry->stack = 0;
  entry->constant_pool_size = 1;
  entry->constant_pool = malloc(sizeof(Constant) * entry->constant_pool_size);

  /* update constant pool function reference */
  caller->constant_pool[0].u.f32_v = 35;
  caller->constant_pool[1].u.f32_v = 42;
  caller->constant_pool[2].u.func_v = callee;
  entry->constant_pool[0].u.func_v = caller;

  program->entry = entry;

  machine = create_machine(100);

  load_program(machine, program);
  run_machine(machine);
  ASSERT_EQUAL((int)(machine->stack[machine->sp].f32_v), 1470);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_program(program);
  free_machine(machine);
}

void test_function_mutual_tail_recursion() {
  Program *program;
  Machine *machine;

  Byte odd_code[] = {/* push local variable n */
                     PUSH_LOCAL_I32, 0, PUSH_I32_0,
                     /* n == 0 */
                     EQ_I32,
                     /* if n <> 0, jump */
                     JUMP_IF_FALSE, 0, 2,
                     /* return false */
                     PUSH_I32_0, RETURN_I32,
                     /* n - 1 */
                     PUSH_LOCAL_I32, 0, PUSH_I32_1, SUB_I32,
                     /* tail call even(n - 1) */
                     TAIL_CALL, 0};
  Byte even_code[] = {/* push local variable n */
                      PUSH_LOCAL_I32, 0, PUSH_I32_0,
                      /* n == 0 */
                      EQ_I32,
                      /* if n <> 0, jump */
                      JUMP_IF_FALSE, 0, 2,
                      /* return true */
                      PUSH_I32_1, RETURN_I32,
                      /* n - 1 */
                      PUSH_LOCAL_I32, 0, PUSH_I32_1, SUB_I32,
                      /* tail call odd(n - 1) */
                      TAIL_CALL, 0};
  Byte entry_code[] = {PUSH_I32, 0, INVOKE_FUNCTION, 1, PUSH_I32_0, HALT};

  Function *odd;
  Function *even;
  Function *entry;

  program = create_program("Program", 0, 0, 3, 0, 0, 2);

  /* odd function */
  odd = &(program->functions[0]);
  copy_byte_code(odd, odd_code, sizeof(odd_code) / sizeof(Byte));
  odd->name = make_string("odd");
  odd->args_size = 1;
  odd->locals = 0;
  odd->stack = 0;
  odd->constant_pool_size = 1;
  odd->constant_pool = malloc(sizeof(Constant) * odd->constant_pool_size);

  /* even function */
  even = &(program->functions[1]);
  copy_byte_code(even, even_code, sizeof(even_code) / sizeof(Byte));
  even->name = make_string("even");
  even->args_size = 1;
  even->locals = 0;
  even->stack = 0;
  even->constant_pool_size = 1;
  even->constant_pool = malloc(sizeof(Constant) * even->constant_pool_size);

  /* entry function */
  entry = &(program->functions[2]);
  copy_byte_code(entry, entry_code, sizeof(entry_code) / sizeof(Byte));
  entry->name = make_string("entry");
  entry->args_size = 0;
  entry->locals = 0;
  entry->stack = 0;
  entry->constant_pool_size = 2;
  entry->constant_pool = malloc(sizeof(Constant) * entry->constant_pool_size);

  /* update constant pool function reference */
  odd->constant_pool[0].u.func_v = even;
  even->constant_pool[0].u.func_v = odd;
  entry->constant_pool[1].u.func_v = even;

  program->entry = entry;

  RUN_EVEN_ODD_PROGRAM_WITH_INPUT(2);
  RUN_EVEN_ODD_PROGRAM_WITH_INPUT(13);
  RUN_EVEN_ODD_PROGRAM_WITH_INPUT(75);
  RUN_EVEN_ODD_PROGRAM_WITH_INPUT(10000);
  RUN_EVEN_ODD_PROGRAM_WITH_INPUT(10007);

  free_program(program);
}