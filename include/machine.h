#ifndef FLINT_VM_MACHINE_H
#define FLINT_VM_MACHINE_H

#include "value.h"

typedef struct Environment {
  Function *function;
  Module *module;
} Environment;

typedef struct Machine {
  /* stack for evaluation */
  i32 stack_max_size;
  Value *stack;

  /* current environment */
  Environment env;

  /* current state */
  i32 sp; /* stack pointer */
  i32 fp; /* function pointer */
  i32 pc; /* program counter */
} Machine;

Machine *create_machine(i32 stack_max_size);

void run_machine(Machine *machine);

void update_machine_state(Machine *machine, i32 sp, i32 fp, i32 pc);

void free_machine(Machine *machine);

#endif