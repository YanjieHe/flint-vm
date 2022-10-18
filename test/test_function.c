#include "test_function.h"
#include "test.h"
#include "opcode.h"
#include "type.h"

void test_function_invocation() {
  Program *program;
  Machine *machine;
  Byte caller_code[] = {/* push first argument */
                        PUSH_I32_1BYTE, 25, CAST_I32_TO_F32,
                        /* push second argument */
                        PUSH_I32_1BYTE, 35, CAST_I32_TO_F32,
                        /* invoke function */
                        INVOKE_FUNCTION, 0};
  Byte callee_code[] = {PUSH_LOCAL_F32, 0, PUSH_LOCAL_F32, 1, MUL_F32};
  Function *caller;
  Function *callee;

  program = malloc(sizeof(Program));

  program->file_name = NULL;
  program->module_count = 1;
  program->modules = malloc(sizeof(Module));
  program->structure_count = 0;
  program->structures = NULL;
  program->modules[0].function_count = 2;
  program->modules[0].functions =
      malloc(sizeof(Function) * program->modules[0].function_count);

  /* caller function */
  caller = &(program->modules[0].functions[0]);
  copy_byte_code(caller, caller_code, sizeof(caller_code) / sizeof(Byte));
  caller->name = make_string("caller");
  caller->constant_pool_size = 1;
  caller->constant_pool = malloc(
      sizeof(Value) * program->modules[0].functions[0].constant_pool_size);

  /* callee function */
  callee = &(program->modules[0].functions[1]);
  copy_byte_code(callee, callee_code, sizeof(callee_code) / sizeof(Byte));
  callee->name = make_string("callee");
  callee->args_size = 2;
  callee->locals = 0;
  callee->constant_pool_size = 0;
  callee->constant_pool = NULL;

  /* update constant pool function reference */
  caller->constant_pool[0].func_v = callee;

  program->entry = caller;

  machine = create_machine(100);

  load_program_on_machine(program, machine, 0, 0);
  run_machine(machine);
  ASSERT_EQUAL((i32)machine->stack[machine->sp].f32_v, 875);

  free_program(program);
  free_machine(machine);
}