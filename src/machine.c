#include "machine.h"
#include "opcode.h"
#include "type.h"
#include <stdio.h>
#include <stdlib.h>

#define STACK_PUSH_I32(VALUE)                                                  \
  sp++;                                                                        \
  stack[sp].u.i32_v = (VALUE);                                                 \
  stack[sp].type = TYPE_I32;

#define STACK_PUSH_I64(VALUE)                                                  \
  sp++;                                                                        \
  stack[sp].u.i64_v = (VALUE);                                                 \
  stack[sp].type = TYPE_I64;

#define STACK_PUSH_F32(VALUE)                                                  \
  sp++;                                                                        \
  stack[sp].u.f32_v = (VALUE);                                                 \
  stack[sp].type = TYPE_F32;

#define STACK_PUSH_F64(VALUE)                                                  \
  sp++;                                                                        \
  stack[sp].u.f64_v = (VALUE);                                                 \
  stack[sp].type = TYPE_F64;

#define STACK_POP_I32(VALUE)                                                   \
  (VALUE) = stack[sp].u.i32_v;                                                 \
  stack[sp].type = TYPE_EMPTY;                                                 \
  sp--;

#define STACK_POP_I64(VALUE)                                                   \
  (VALUE) = stack[sp].u.i64_v;                                                 \
  stack[sp].type = TYPE_EMPTY;                                                 \
  sp--;

#define STACK_POP_F32(VALUE)                                                   \
  (VALUE) = stack[sp].u.f32_v;                                                 \
  stack[sp].type = TYPE_EMPTY;                                                 \
  sp--;

#define STACK_POP_F64(VALUE)                                                   \
  (VALUE) = stack[sp].u.f64_v;                                                 \
  stack[sp].type = TYPE_EMPTY;                                                 \
  sp--;

Machine *create_machine(i32 stack_max_size) {
  Machine *machine;

  machine = malloc(sizeof(Machine));
  machine->stack_max_size = stack_max_size;
  machine->stack = malloc(sizeof(Value) * stack_max_size);
  machine->env.function = NULL;
  machine->env.module = NULL;
  machine->sp = -1;
  machine->fp = 0;
  machine->pc = 0;

  return machine;
}

void run_machine(Machine *machine) {
  /* state */
  Byte *code;
  i32 code_length;
  Byte op;
  Value *stack;
  i32 sp;
  i32 fp;
  i32 pc;

  /* type comparison for binary operations */
  u32 left_type;
  u32 right_type;

  code = machine->env.function->code;
  code_length = machine->env.function->code_length;
  sp = machine->sp;
  fp = machine->fp;
  pc = machine->pc;

  printf("code length: %d\n", code_length);

  while (pc < code_length) {

    op = code[pc];
    pc++;
    printf("op = %s\n", opcode_info[op][0]);

    switch (op) {
    case PUSH_I32_0: {
      STACK_PUSH_I32(0);
      break;
    }
    case PUSH_I32_1: {
      STACK_PUSH_I32(1);
      break;
    }
    case ADD_I32: {
      stack[sp - 1].u.i32_v = (stack[sp - 1].u.i32_v + stack[sp].u.i32_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case SUB_I32: {
      stack[sp - 1].u.i32_v = (stack[sp - 1].u.i32_v - stack[sp].u.i32_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case MUL_I32: {
      stack[sp - 1].u.i32_v = (stack[sp - 1].u.i32_v * stack[sp].u.i32_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case DIV_I32: {
      stack[sp - 1].u.i32_v = (stack[sp - 1].u.i32_v / stack[sp].u.i32_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case ADD_I64: {
      stack[sp - 1].u.i64_v = (stack[sp - 1].u.i64_v + stack[sp].u.i64_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case SUB_I64: {
      stack[sp - 1].u.i64_v = (stack[sp - 1].u.i64_v - stack[sp].u.i64_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case MUL_I64: {
      stack[sp - 1].u.i64_v = (stack[sp - 1].u.i64_v * stack[sp].u.i64_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case DIV_I64: {
      stack[sp - 1].u.i64_v = (stack[sp - 1].u.i64_v / stack[sp].u.i64_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case ADD_F32: {
      stack[sp - 1].u.f32_v = (stack[sp - 1].u.f32_v + stack[sp].u.f32_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case SUB_F32: {
      stack[sp - 1].u.f32_v = (stack[sp - 1].u.f32_v - stack[sp].u.f32_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case MUL_F32: {
      stack[sp - 1].u.f32_v = (stack[sp - 1].u.f32_v * stack[sp].u.f32_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case DIV_F32: {
      stack[sp - 1].u.f32_v = (stack[sp - 1].u.f32_v / stack[sp].u.f32_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case ADD_F64: {
      stack[sp - 1].u.f64_v = (stack[sp - 1].u.f64_v + stack[sp].u.f64_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case SUB_F64: {
      stack[sp - 1].u.f64_v = (stack[sp - 1].u.f64_v - stack[sp].u.f64_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case MUL_F64: {
      stack[sp - 1].u.f64_v = (stack[sp - 1].u.f64_v * stack[sp].u.f64_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case DIV_F64: {
      stack[sp - 1].u.f64_v = (stack[sp - 1].u.f64_v / stack[sp].u.f64_v);
      stack[sp].type = TYPE_EMPTY;
      sp--;
      break;
    }
    case ADD_DYNAMIC: {
      left_type = stack[sp - 1].type;
      right_type = stack[sp].type;
      if (left_type == right_type) {
        switch (left_type) {
        case TYPE_I32: {
          stack[sp - 1].u.i32_v = (stack[sp - 1].u.i32_v + stack[sp].u.i32_v);
          break;
        }
        case TYPE_I64: {
          stack[sp - 1].u.i64_v = (stack[sp - 1].u.i64_v + stack[sp].u.i64_v);
          break;
        }
        case TYPE_F32: {
          stack[sp - 1].u.f32_v = (stack[sp - 1].u.f32_v + stack[sp].u.f32_v);
          break;
        }
        case TYPE_F64: {
          stack[sp - 1].u.f64_v = (stack[sp - 1].u.f64_v + stack[sp].u.f64_v);
          break;
        }
        default: {
          /* throw error */
          break;
        }
          stack[sp].type = TYPE_EMPTY;
          sp--;
        }
      } else {
        /* throw error */
      }
      break;
    }
    }
  }
}