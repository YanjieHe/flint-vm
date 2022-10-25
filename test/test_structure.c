#include "test_structure.h"
#include "test.h"
#include "opcode.h"
#include "type.h"
#include "byte_code_loader.h"

void test_structure() {
  Program *program;
  Machine *machine;
  Function *function;
  Byte code[] = {
      /* create a new structure */
      NEW, 0,
      /* copy the structure */
      DUPLICATE,
      /* push the value for the first field */
      PUSH_I64, 1,
      /* assign the value to the first field */
      POP_FIELD_I64, 0,
      /* copy the structure */
      DUPLICATE,
      /* push the value for the second field */
      PUSH_F64, 2,
      /* assign the value to the second field */
      POP_FIELD_F64, 1,
  };

  program = malloc(sizeof(Program));

  program->file_name = NULL;
  program->module_count = 1;
  program->modules = malloc(sizeof(Module));

  program->structure_count = 1;
  program->structures_meta_data = malloc(sizeof(StructureMetaData));
  program->structures_meta_data[0].n_values = 2;
  program->structures_meta_data[0].name = make_string("Structure");

  init_module(&(program->modules[0]));
  program->modules[0].function_count = 1;
  program->modules[0].functions = malloc(sizeof(Function));
  function = &(program->modules[0].functions[0]);
  copy_byte_code(function, code, sizeof(code) / sizeof(Byte));
  function->name = make_string(__FUNCTION__);
  function->constant_pool_size = 3;
  function->constant_pool =
      malloc(sizeof(Constant) * function->constant_pool_size);
  function->constant_pool[0].u.struct_meta_data = &(program->structures_meta_data[0]);
  function->constant_pool[1].u.i64_v = 53;
  function->constant_pool[2].u.f64_v = 2.89;
  program->entry = function;

  machine = create_machine(100);

  load_program_on_machine(program, machine, 0, 0);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].obj_v->u.struct_v->values[0].i64_v,
               53);
  ASSERT_EQUAL(machine->machine_status, MACHINE_COMPLETED);

  free_program(program);
  free_machine(machine);
}