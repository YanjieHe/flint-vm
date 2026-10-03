#include "test_interface.h"
#include "byte_code_loader.h"
#include "machine.h"
#include "opcode.h"
#include "test.h"
#include "value.h"

static void initialize_i32_method(Function *function, const char *name,
                                  Byte result) {
  Byte code[3];

  code[0] = PUSH_I32_1BYTE;
  code[1] = result;
  code[2] = RETURN_I32;

  copy_byte_code(function, code, sizeof(code) / sizeof(Byte));
  function->name = make_string(name);
  function->args_size = 1;
  function->locals = 0;
  function->stack = 0;
}

static void initialize_interface(InterfaceMetaData *interface_meta,
                                 i32 interface_index, const char *name,
                                 u16 method_count) {
  int i;

  interface_meta->interface_index = interface_index;
  interface_meta->name = make_string(name);
  interface_meta->method_count = method_count;
  interface_meta->methods =
      malloc(sizeof(InterfaceMethodMetaData) * method_count);
  for (i = 0; i < method_count; i++) {
    interface_meta->methods[i].name = NULL;
    interface_meta->methods[i].args_size = 0;
  }
}

static void initialize_vtable_entry(VTableEntry *entry, i32 interface_index,
                                    u16 method_count) {
  int i;

  entry->interface_index = interface_index;
  entry->method_count = method_count;
  entry->methods = malloc(sizeof(Function *) * method_count);
  for (i = 0; i < method_count; i++) {
    entry->methods[i] = NULL;
  }
}

void test_interface_vtable() {
  Program *program;
  StructureMetaData *paladin;

  program = create_program("Program", 0, 1, 4, 0, 0, 0, 0);

  paladin = &(program->structures_meta_data[0]);
  paladin->name = make_string("Paladin");
  paladin->vtable_entry_count = 2;
  paladin->vtable_entries = malloc(sizeof(VTableEntry) * 2);

  paladin->vtable_entries[0].interface_index = 0;
  paladin->vtable_entries[0].method_count = 2;
  paladin->vtable_entries[0].methods = malloc(sizeof(Function *) * 2);
  paladin->vtable_entries[0].methods[0] = &(program->functions[1]);
  paladin->vtable_entries[0].methods[1] = &(program->functions[2]);

  paladin->vtable_entries[1].interface_index = 1;
  paladin->vtable_entries[1].method_count = 1;
  paladin->vtable_entries[1].methods = malloc(sizeof(Function *));
  paladin->vtable_entries[1].methods[0] = &(program->functions[3]);

  ASSERT_EQUAL(paladin->vtable_entry_count, 2);
  ASSERT_EQUAL(paladin->vtable_entries[0].interface_index, 0);
  ASSERT_EQUAL(paladin->vtable_entries[0].method_count, 2);
  ASSERT_EQUAL(paladin->vtable_entries[0].methods[0], &(program->functions[1]));
  ASSERT_EQUAL(paladin->vtable_entries[0].methods[1], &(program->functions[2]));
  ASSERT_EQUAL(paladin->vtable_entries[1].interface_index, 1);
  ASSERT_EQUAL(paladin->vtable_entries[1].method_count, 1);
  ASSERT_EQUAL(paladin->vtable_entries[1].methods[0], &(program->functions[3]));

  free_program(program);
}

void test_resolve_interface_method_references() {
  Program *program;
  Function *function;
  InterfaceMethodReference *defend_reference;
  InterfaceMethodReference *heal_reference;

  program = create_program("Program", 0, 0, 1, 0, 0, 2, 0);

  program->interfaces_meta_data[0].interface_index = 0;
  program->interfaces_meta_data[0].method_count = 2;
  program->interfaces_meta_data[0].methods =
      malloc(sizeof(InterfaceMethodMetaData) * 2);
  program->interfaces_meta_data[0].methods[0].name = NULL;
  program->interfaces_meta_data[0].methods[0].args_size = 2;
  program->interfaces_meta_data[0].methods[1].name = NULL;
  program->interfaces_meta_data[0].methods[1].args_size = 1;

  program->interfaces_meta_data[1].interface_index = 1;
  program->interfaces_meta_data[1].method_count = 1;
  program->interfaces_meta_data[1].methods =
      malloc(sizeof(InterfaceMethodMetaData));
  program->interfaces_meta_data[1].methods[0].name = NULL;
  program->interfaces_meta_data[1].methods[0].args_size = 3;

  defend_reference = malloc(sizeof(InterfaceMethodReference));
  defend_reference->interface_index = 0;
  defend_reference->method_index = 1;
  defend_reference->args_size = 0;

  heal_reference = malloc(sizeof(InterfaceMethodReference));
  heal_reference->interface_index = 1;
  heal_reference->method_index = 0;
  heal_reference->args_size = 0;

  function = &(program->functions[0]);
  function->constant_pool_size = 2;
  function->constant_pool = malloc(sizeof(Constant) * 2);
  function->constant_pool[0].kind =
      CONSTANT_KIND_INTERFACE_METHOD_REFERENCE;
  function->constant_pool[0].u.interface_method_ref_v = defend_reference;
  function->constant_pool[1].kind =
      CONSTANT_KIND_INTERFACE_METHOD_REFERENCE;
  function->constant_pool[1].u.interface_method_ref_v = heal_reference;

  resolve_interface_method_references(program);

  ASSERT_EQUAL(defend_reference->interface_index, 0);
  ASSERT_EQUAL(defend_reference->method_index, 1);
  ASSERT_EQUAL(defend_reference->args_size, 1);
  ASSERT_EQUAL(heal_reference->interface_index, 1);
  ASSERT_EQUAL(heal_reference->method_index, 0);
  ASSERT_EQUAL(heal_reference->args_size, 3);

  free_program(program);
}

void test_invoke_interface() {
  Program *program;
  Machine *machine;
  Function *entry;
  Function *add_method;
  StructureMetaData *calculator;
  InterfaceMetaData *adder;
  InterfaceMethodReference *add_reference;
  Byte entry_code[] = {NEW,
                       1,
                       PUSH_I32_1BYTE,
                       17,
                       PUSH_I32_1BYTE,
                       25,
                       INVOKE_INTERFACE,
                       0,
                       PUSH_I32_0,
                       HALT};
  Byte add_code[] = {PUSH_LOCAL_I32, 1, PUSH_LOCAL_I32, 2, ADD_I32,
                     RETURN_I32};

  program = create_program("Program", 0, 1, 2, 0, 0, 1, 0);

  add_method = &(program->functions[1]);
  copy_byte_code(add_method, add_code, sizeof(add_code) / sizeof(Byte));
  add_method->name = make_string("add");
  add_method->args_size = 3;
  add_method->locals = 0;
  add_method->stack = 0;

  calculator = &(program->structures_meta_data[0]);
  calculator->name = make_string("Calculator");
  calculator->vtable_entry_count = 1;
  calculator->vtable_entries = malloc(sizeof(VTableEntry));
  calculator->vtable_entries[0].interface_index = 0;
  calculator->vtable_entries[0].method_count = 1;
  calculator->vtable_entries[0].methods = malloc(sizeof(Function *));
  calculator->vtable_entries[0].methods[0] = add_method;

  adder = &(program->interfaces_meta_data[0]);
  adder->interface_index = 0;
  adder->name = make_string("Adder");
  adder->method_count = 1;
  adder->methods = malloc(sizeof(InterfaceMethodMetaData));
  adder->methods[0].name = make_string("add");
  adder->methods[0].args_size = 3;

  add_reference = malloc(sizeof(InterfaceMethodReference));
  add_reference->interface_index = 0;
  add_reference->method_index = 0;
  add_reference->args_size = 0;

  entry = &(program->functions[0]);
  copy_byte_code(entry, entry_code, sizeof(entry_code) / sizeof(Byte));
  entry->name = make_string(__FUNCTION__);
  entry->constant_pool_size = 2;
  entry->constant_pool = malloc(sizeof(Constant) * 2);
  entry->constant_pool[0].kind =
      CONSTANT_KIND_INTERFACE_METHOD_REFERENCE;
  entry->constant_pool[0].u.interface_method_ref_v = add_reference;
  entry->constant_pool[1].kind = CONSTANT_KIND_STRUCTURE_META_DATA;
  entry->constant_pool[1].u.struct_meta_data = calculator;

  resolve_interface_method_references(program);
  ASSERT_EQUAL(add_reference->args_size, 3);

  machine = create_machine(100);
  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].i32_v, 42);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_program(program);
  free_machine(machine);
}

void test_interface_dispatches_by_receiver_type() {
  Program *program;
  Machine *machine;
  Function *entry;
  Function *warrior_attack;
  Function *mage_attack;
  StructureMetaData *warrior;
  StructureMetaData *mage;
  InterfaceMetaData *combatant;
  InterfaceMethodReference *attack_reference;
  Byte entry_code[] = {NEW,
                       1,
                       INVOKE_INTERFACE,
                       0,
                       NEW,
                       2,
                       INVOKE_INTERFACE,
                       0,
                       PUSH_I32_0,
                       HALT};

  program = create_program("Program", 0, 2, 3, 0, 0, 1, 0);

  warrior_attack = &(program->functions[1]);
  initialize_i32_method(warrior_attack, "Warrior::attack", 10);
  mage_attack = &(program->functions[2]);
  initialize_i32_method(mage_attack, "Mage::attack", 20);

  warrior = &(program->structures_meta_data[0]);
  warrior->name = make_string("Warrior");
  warrior->vtable_entry_count = 1;
  warrior->vtable_entries = malloc(sizeof(VTableEntry));
  initialize_vtable_entry(&(warrior->vtable_entries[0]), 0, 1);
  warrior->vtable_entries[0].methods[0] = warrior_attack;

  mage = &(program->structures_meta_data[1]);
  mage->name = make_string("Mage");
  mage->vtable_entry_count = 1;
  mage->vtable_entries = malloc(sizeof(VTableEntry));
  initialize_vtable_entry(&(mage->vtable_entries[0]), 0, 1);
  mage->vtable_entries[0].methods[0] = mage_attack;

  combatant = &(program->interfaces_meta_data[0]);
  initialize_interface(combatant, 0, "Combatant", 1);
  combatant->methods[0].name = make_string("attack");
  combatant->methods[0].args_size = 1;

  attack_reference = malloc(sizeof(InterfaceMethodReference));
  attack_reference->interface_index = 0;
  attack_reference->method_index = 0;
  attack_reference->args_size = 0;

  entry = &(program->functions[0]);
  copy_byte_code(entry, entry_code, sizeof(entry_code) / sizeof(Byte));
  entry->name = make_string(__FUNCTION__);
  entry->constant_pool_size = 3;
  entry->constant_pool = malloc(sizeof(Constant) * 3);
  entry->constant_pool[0].kind =
      CONSTANT_KIND_INTERFACE_METHOD_REFERENCE;
  entry->constant_pool[0].u.interface_method_ref_v = attack_reference;
  entry->constant_pool[1].kind = CONSTANT_KIND_STRUCTURE_META_DATA;
  entry->constant_pool[1].u.struct_meta_data = warrior;
  entry->constant_pool[2].kind = CONSTANT_KIND_STRUCTURE_META_DATA;
  entry->constant_pool[2].u.struct_meta_data = mage;

  resolve_interface_method_references(program);

  machine = create_machine(100);
  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp - 1].i32_v, 10);
  ASSERT_EQUAL(machine->stack[machine->sp].i32_v, 20);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_program(program);
  free_machine(machine);
}

void test_interface_dispatches_nonzero_method_index() {
  Program *program;
  Machine *machine;
  Function *entry;
  Function *attack_method;
  Function *defend_method;
  StructureMetaData *guardian;
  InterfaceMetaData *combatant;
  InterfaceMethodReference *defend_reference;
  Byte entry_code[] = {NEW, 1, INVOKE_INTERFACE, 0, PUSH_I32_0, HALT};

  program = create_program("Program", 0, 1, 3, 0, 0, 1, 0);

  attack_method = &(program->functions[1]);
  initialize_i32_method(attack_method, "Guardian::attack", 10);
  defend_method = &(program->functions[2]);
  initialize_i32_method(defend_method, "Guardian::defend", 30);

  guardian = &(program->structures_meta_data[0]);
  guardian->name = make_string("Guardian");
  guardian->vtable_entry_count = 1;
  guardian->vtable_entries = malloc(sizeof(VTableEntry));
  initialize_vtable_entry(&(guardian->vtable_entries[0]), 0, 2);
  guardian->vtable_entries[0].methods[0] = attack_method;
  guardian->vtable_entries[0].methods[1] = defend_method;

  combatant = &(program->interfaces_meta_data[0]);
  initialize_interface(combatant, 0, "Combatant", 2);
  combatant->methods[0].name = make_string("attack");
  combatant->methods[0].args_size = 1;
  combatant->methods[1].name = make_string("defend");
  combatant->methods[1].args_size = 1;

  defend_reference = malloc(sizeof(InterfaceMethodReference));
  defend_reference->interface_index = 0;
  defend_reference->method_index = 1;
  defend_reference->args_size = 0;

  entry = &(program->functions[0]);
  copy_byte_code(entry, entry_code, sizeof(entry_code) / sizeof(Byte));
  entry->name = make_string(__FUNCTION__);
  entry->constant_pool_size = 2;
  entry->constant_pool = malloc(sizeof(Constant) * 2);
  entry->constant_pool[0].kind =
      CONSTANT_KIND_INTERFACE_METHOD_REFERENCE;
  entry->constant_pool[0].u.interface_method_ref_v = defend_reference;
  entry->constant_pool[1].kind = CONSTANT_KIND_STRUCTURE_META_DATA;
  entry->constant_pool[1].u.struct_meta_data = guardian;

  resolve_interface_method_references(program);

  machine = create_machine(100);
  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].i32_v, 30);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_program(program);
  free_machine(machine);
}

void test_interface_dispatches_multiple_interfaces() {
  Program *program;
  Machine *machine;
  Function *entry;
  Function *attack_method;
  Function *heal_method;
  StructureMetaData *paladin;
  InterfaceMetaData *combatant;
  InterfaceMetaData *healer;
  InterfaceMethodReference *heal_reference;
  Byte entry_code[] = {NEW, 1, INVOKE_INTERFACE, 0, PUSH_I32_0, HALT};

  program = create_program("Program", 0, 1, 3, 0, 0, 2, 0);

  attack_method = &(program->functions[1]);
  initialize_i32_method(attack_method, "Paladin::attack", 10);
  heal_method = &(program->functions[2]);
  initialize_i32_method(heal_method, "Paladin::heal", 50);

  paladin = &(program->structures_meta_data[0]);
  paladin->name = make_string("Paladin");
  paladin->vtable_entry_count = 2;
  paladin->vtable_entries = malloc(sizeof(VTableEntry) * 2);
  initialize_vtable_entry(&(paladin->vtable_entries[0]), 0, 1);
  paladin->vtable_entries[0].methods[0] = attack_method;
  initialize_vtable_entry(&(paladin->vtable_entries[1]), 1, 1);
  paladin->vtable_entries[1].methods[0] = heal_method;

  combatant = &(program->interfaces_meta_data[0]);
  initialize_interface(combatant, 0, "Combatant", 1);
  combatant->methods[0].name = make_string("attack");
  combatant->methods[0].args_size = 1;

  healer = &(program->interfaces_meta_data[1]);
  initialize_interface(healer, 1, "Healer", 1);
  healer->methods[0].name = make_string("heal");
  healer->methods[0].args_size = 1;

  heal_reference = malloc(sizeof(InterfaceMethodReference));
  heal_reference->interface_index = 1;
  heal_reference->method_index = 0;
  heal_reference->args_size = 0;

  entry = &(program->functions[0]);
  copy_byte_code(entry, entry_code, sizeof(entry_code) / sizeof(Byte));
  entry->name = make_string(__FUNCTION__);
  entry->constant_pool_size = 2;
  entry->constant_pool = malloc(sizeof(Constant) * 2);
  entry->constant_pool[0].kind =
      CONSTANT_KIND_INTERFACE_METHOD_REFERENCE;
  entry->constant_pool[0].u.interface_method_ref_v = heal_reference;
  entry->constant_pool[1].kind = CONSTANT_KIND_STRUCTURE_META_DATA;
  entry->constant_pool[1].u.struct_meta_data = paladin;

  resolve_interface_method_references(program);

  machine = create_machine(100);
  load_program(machine, program);
  run_machine(machine);

  ASSERT_EQUAL(machine->stack[machine->sp].i32_v, 50);
  ASSERT_EQUAL(machine->machine_status, MACHINE_STOPPED);

  free_program(program);
  free_machine(machine);
}
