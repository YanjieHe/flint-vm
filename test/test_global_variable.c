#include "test_global_variable.h"
#include "test.h"
#include "opcode.h"
#include "type.h"
#include "byte_code_loader.h"

void test_global_variable() {
  Program *program;
  Machine *machine;
  Byte global_variable_initialization_code[] = {PUSH_F64_1,
                                                /* invoke function */
                                                POP_GLOBAL_F64, 0,
                                                /* return */
                                                RETURN};
  Byte entry_function_code[] = {PUSH_GLOBAL_F64, 0, PUSH_F64, 1, ADD_F64, HALT};
  Function *entry;
  Function *initializer;

  program = create_program("Program", 1, 0, 2, 0, 0);

  /* entry function */
  entry = &(program->functions[0]);
  copy_byte_code(entry, entry_function_code,
                 sizeof(entry_function_code) / sizeof(Byte));
  entry->name = make_string("entry");
  entry->args_size = 0;
  entry->locals = 0;
  entry->stack = 0;
  entry->constant_pool_size = 2;
  entry->constant_pool = malloc(sizeof(Constant) * entry->constant_pool_size);

  /* initializer function */
  initializer = &(program->functions[1]);
  copy_byte_code(initializer, global_variable_initialization_code,
                 sizeof(global_variable_initialization_code) / sizeof(Byte));
  initializer->name = make_string("initializer");
  initializer->args_size = 0;
  initializer->locals = 0;
  initializer->stack = 0;
  initializer->constant_pool_size = 1;
  initializer->constant_pool =
      malloc(sizeof(Constant) * initializer->constant_pool_size);

  program->global_variables[0].is_initialized = FALSE;
  program->global_variables[0].initializer = initializer;
  program->global_variables[0].name = make_string("A");

  entry->constant_pool[0].u.global_variable_v = &(program->global_variables[0]);
  entry->constant_pool[1].u.f64_v = 17.0;
  initializer->constant_pool[0].u.global_variable_v =
      &(program->global_variables[0]);

  program->entry = entry;

  machine = create_machine(100);

  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);
  ASSERT_EQUAL((int)(machine->stack[machine->sp].f64_v), 18);

  free_program(program);
  free_machine(machine);
}