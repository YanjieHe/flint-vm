#ifndef FLINT_VM_MACHINE_H
#define FLINT_VM_MACHINE_H

#include "value.h"

typedef struct Environment { Function *function; } Environment;

enum MachineStatus {
  MACHINE_STOPPED,
  MACHINE_RUNNING,

  /* runtime errors */
  RUNTIME_ERROR_ARRAY_LENGTH_LESS_THAN_ZERO,
  RUNTIME_ERROR_NATIVE_FUNCTION_ERROR
};

typedef struct Machine {
  /* stack for evaluation */
  i32 stack_max_size;
  Value *stack;
  u8 *is_gc_object;
  GCObject *heap;

  /* current environment */
  Environment env;

  /* current state */
  i32 sp;   /* stack pointer */
  i32 fp;   /* function pointer */
  Byte *pc; /* program counter */

  /* status code */
  i32 machine_status;
} Machine;

Machine *create_machine(i32 stack_max_size);

void free_machine(Machine *machine);

void load_program(Machine *machine, Program *program);

void run_machine(Machine *machine);

#define GET_I32_ARG(MACHINE, OFFSET)                                           \
  ((MACHINE)->stack[(MACHINE)->fp + (OFFSET)].i32_v)
#define GET_I64_ARG(MACHINE, OFFSET)                                           \
  ((MACHINE)->stack[(MACHINE)->fp + (OFFSET)].i64_v)
#define GET_F32_ARG(MACHINE, OFFSET)                                           \
  ((MACHINE)->stack[(MACHINE)->fp + (OFFSET)].f32_v)
#define GET_F64_ARG(MACHINE, OFFSET)                                           \
  ((MACHINE)->stack[(MACHINE)->fp + (OFFSET)].f64_v)
#define GET_STRUCT_ARG(MACHINE, OFFSET)                                        \
  ((MACHINE)->stack[(MACHINE)->fp + (OFFSET)].obj_v->u.struct_v)
#define GET_STRING_ARG(MACHINE, OFFSET)                                        \
  ((MACHINE)->stack[(MACHINE)->fp + (OFFSET)].obj_v->u.str_v)

#endif