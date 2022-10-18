#ifndef FLINT_VM_MACHINE_H
#define FLINT_VM_MACHINE_H

#include "value.h"

typedef struct Environment {
  Function *function;
  Module *module;
} Environment;

enum MachineStatus {
  MACHINE_STOPPED,
  MACHINE_RUNNING,
  MACHINE_COMPLETED,

  /* runtime errors */
  RUNTIME_ERROR_ARRAY_LENGTH_LESS_THAN_ZERO
};

typedef struct Machine {
  /* stack for evaluation */
  i32 stack_max_size;
  Value *stack;
  u8* is_gc_object;
  GCObject* heap;

  /* current environment */
  Environment env;

  /* current state */
  i32 sp; /* stack pointer */
  i32 fp; /* function pointer */
  i32 pc; /* program counter */

  /* status code */
  i32 machine_status;
} Machine;

Machine *create_machine(i32 stack_max_size);

void run_machine(Machine *machine);

void update_machine_state(Machine *machine, i32 sp, i32 fp, i32 pc);

void free_machine(Machine *machine);

#endif