#include "test_structure.h"
#include "test.h"
#include "opcode.h"
#include "type.h"
#include "byte_code_loader.h"
#include "math.h"

void test_structure() {
  Program *program;
  Machine *machine;
  Function *function;
  Byte code[] = {/* create a new structure */
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
                 POP_FIELD_F64, 1, HALT};

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
  function->constant_pool[0].u.struct_meta_data =
      &(program->structures_meta_data[0]);
  function->constant_pool[1].u.i64_v = 53;
  function->constant_pool[2].u.f64_v = 2.89;
  program->entry = function;

  machine = create_machine(100);

  load_program_on_machine(program, machine, 0, 0);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].obj_v->u.struct_v->values[0].i64_v,
               53);
  ASSERT_EQUAL(
      abs(machine->stack[machine->sp].obj_v->u.struct_v->values[1].f64_v -
          2.89) < 0.0000001,
      TRUE);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_program(program);
  free_machine(machine);
}

void test_structure_get_field_value() {
  Program *program;
  Machine *machine;
  Function *function;
  Byte code[] = {/* create a new structure */
                 NEW, 0,
                 /* copy the structure */
                 DUPLICATE,
                 /* push the value for the first field */
                 PUSH_F32, 1,
                 /* assign the value to the first field */
                 POP_FIELD_F32, 0,
                 /* copy the structure */
                 DUPLICATE,
                 /* push the value for the second field */
                 PUSH_STRING, 2,
                 /* assign the value to the second field */
                 POP_FIELD_OBJECT, 1,
                 /* store the newly-created object */
                 POP_LOCAL_OBJECT, 0,
                 /* push the first field of the object */
                 PUSH_LOCAL_OBJECT, 0, PUSH_FIELD_F32, 0,
                 /* push the second field of the object */
                 PUSH_LOCAL_OBJECT, 0, PUSH_FIELD_OBJECT, 1, HALT};

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
  function->constant_pool[0].u.struct_meta_data =
      &(program->structures_meta_data[0]);
  function->constant_pool[1].u.f32_v = 7.04F;
  function->constant_pool[2].u.obj_v = malloc(sizeof(GCObject));
  function->constant_pool[2].u.obj_v->kind = GCOBJECT_KIND_STRING;
  function->constant_pool[2].u.obj_v->next = NULL;
  function->constant_pool[2].u.obj_v->u.str_v =
      make_string("This is the second field of the structure.");
  program->entry = function;

  machine = create_machine(100);

  load_program_on_machine(program, machine, 0, 0);
  machine->sp++;
  run_machine(machine);

  ASSERT_EQUAL(abs(machine->stack[machine->sp - 1].f32_v - 7.04F) < 0.0000001F,
               TRUE);
  ASSERT_EQUAL(machine->stack[machine->sp].obj_v->kind, GCOBJECT_KIND_STRING);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_gc_object(function->constant_pool[2].u.obj_v);

  free_program(program);
  free_machine(machine);
}