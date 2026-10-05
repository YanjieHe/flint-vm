#ifndef FLINT_VM_MACHINE_H
#define FLINT_VM_MACHINE_H

#include "value.h"
#include <string.h>

typedef struct Environment {
  Function *function;
} Environment;

enum MachineStatus {
  MACHINE_STOPPED,
  MACHINE_RUNNING,

  /* runtime errors */
  RUNTIME_ERROR_ARRAY_LENGTH_LESS_THAN_ZERO,
  RUNTIME_ERROR_ARRAY_INDEX_OUT_OF_RANGE,
  RUNTIME_ERROR_STRING_INDEX_OUT_OF_RANGE,
  RUNTIME_ERROR_NATIVE_FUNCTION_ERROR,
  RUNTIME_ERROR_INTERFACE_METHOD_NOT_FOUND
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

void print_stack(Machine *machine, i32 size);

i32 run_machine(Machine *machine);

enum NativeFunctionStatusCode {
  NATIVE_FUNCTION_SUCCESS = 0,
  NATIVE_FUNCTION_ERROR = 1
};

#define FLINT_GET_I32_ARG(MACHINE, INDEX)                                      \
  ((MACHINE)->stack[(MACHINE)->fp + (INDEX)].i32_v)
#define FLINT_GET_I64_ARG(MACHINE, INDEX)                                      \
  ((MACHINE)->stack[(MACHINE)->fp + (INDEX)].i64_v)
#define FLINT_GET_F32_ARG(MACHINE, INDEX)                                      \
  ((MACHINE)->stack[(MACHINE)->fp + (INDEX)].f32_v)
#define FLINT_GET_F64_ARG(MACHINE, INDEX)                                      \
  ((MACHINE)->stack[(MACHINE)->fp + (INDEX)].f64_v)
#define FLINT_GET_BOOL_ARG(MACHINE, INDEX)                                     \
  ((BOOLEAN)((MACHINE)->stack[(MACHINE)->fp + (INDEX)].i32_v))
#define FLINT_GET_CHAR_ARG(MACHINE, INDEX)                                     \
  ((uint32_t)((MACHINE)->stack[(MACHINE)->fp + (INDEX)].i32_v))

#define FLINT_GET_OBJECT_ARG(MACHINE, INDEX)                                   \
  ((MACHINE)->stack[(MACHINE)->fp + (INDEX)].obj_v)
#define FLINT_GET_STRUCT_ARG(MACHINE, INDEX)                                   \
  (FLINT_GET_OBJECT_ARG((MACHINE), (INDEX))->u.struct_v)
#define FLINT_GET_STRING_ARG(MACHINE, INDEX)                                   \
  (FLINT_GET_OBJECT_ARG((MACHINE), (INDEX))->u.str_v)
#define FLINT_GET_ARRAY_ARG(MACHINE, INDEX)                                    \
  (FLINT_GET_OBJECT_ARG((MACHINE), (INDEX))->u.arr_v)

#define FLINT_CLEAR_NATIVE_ARGUMENTS(MACHINE)                                  \
  do {                                                                         \
    if ((MACHINE)->sp >= (MACHINE)->fp) {                                      \
      memset((MACHINE)->is_gc_object + (MACHINE)->fp, 0,                       \
             (size_t)((MACHINE)->sp - (MACHINE)->fp + 1));                     \
    }                                                                          \
  } while (0)

#define FLINT_RETURN_VOID(MACHINE)                                             \
  do {                                                                         \
    FLINT_CLEAR_NATIVE_ARGUMENTS(MACHINE);                                     \
    (MACHINE)->is_gc_object[(MACHINE)->fp] = FALSE;                            \
    (MACHINE)->sp = (MACHINE)->fp;                                             \
    return NATIVE_FUNCTION_SUCCESS;                                            \
  } while (0)

#define FLINT_RETURN_I32(MACHINE, VALUE)                                       \
  do {                                                                         \
    FLINT_CLEAR_NATIVE_ARGUMENTS(MACHINE);                                     \
    (MACHINE)->stack[(MACHINE)->fp].i32_v = (VALUE);                           \
    (MACHINE)->is_gc_object[(MACHINE)->fp] = FALSE;                            \
    (MACHINE)->sp = (MACHINE)->fp;                                             \
    return NATIVE_FUNCTION_SUCCESS;                                            \
  } while (0)

#define FLINT_RETURN_I64(MACHINE, VALUE)                                       \
  do {                                                                         \
    FLINT_CLEAR_NATIVE_ARGUMENTS(MACHINE);                                     \
    (MACHINE)->stack[(MACHINE)->fp].i64_v = (VALUE);                           \
    (MACHINE)->is_gc_object[(MACHINE)->fp] = FALSE;                            \
    (MACHINE)->sp = (MACHINE)->fp;                                             \
    return NATIVE_FUNCTION_SUCCESS;                                            \
  } while (0)

#define FLINT_RETURN_F32(MACHINE, VALUE)                                       \
  do {                                                                         \
    FLINT_CLEAR_NATIVE_ARGUMENTS(MACHINE);                                     \
    (MACHINE)->stack[(MACHINE)->fp].f32_v = (VALUE);                           \
    (MACHINE)->is_gc_object[(MACHINE)->fp] = FALSE;                            \
    (MACHINE)->sp = (MACHINE)->fp;                                             \
    return NATIVE_FUNCTION_SUCCESS;                                            \
  } while (0)

#define FLINT_RETURN_F64(MACHINE, VALUE)                                       \
  do {                                                                         \
    FLINT_CLEAR_NATIVE_ARGUMENTS(MACHINE);                                     \
    (MACHINE)->stack[(MACHINE)->fp].f64_v = (VALUE);                           \
    (MACHINE)->is_gc_object[(MACHINE)->fp] = FALSE;                            \
    (MACHINE)->sp = (MACHINE)->fp;                                             \
    return NATIVE_FUNCTION_SUCCESS;                                            \
  } while (0)

#define FLINT_RETURN_OBJECT(MACHINE, VALUE)                                    \
  do {                                                                         \
    FLINT_CLEAR_NATIVE_ARGUMENTS(MACHINE);                                     \
    (MACHINE)->stack[(MACHINE)->fp].obj_v = (VALUE);                           \
    (MACHINE)->is_gc_object[(MACHINE)->fp] = TRUE;                             \
    (MACHINE)->sp = (MACHINE)->fp;                                             \
    return NATIVE_FUNCTION_SUCCESS;                                            \
  } while (0)

#define FLINT_RETURN_BOOL(MACHINE, VALUE)                                      \
  FLINT_RETURN_I32((MACHINE), (VALUE) ? TRUE : FALSE)

#define FLINT_RETURN_CHAR(MACHINE, VALUE)                                      \
  FLINT_RETURN_I32((MACHINE), (i32)(VALUE))

#endif
