#include "machine.h"
#include "opcode.h"
#include "type.h"
#include <stdio.h>
#include <stdlib.h>

#define STACK_PUSH_I32(VALUE)                                                  \
  sp++;                                                                        \
  stack[sp].i32_v = (VALUE);

#define STACK_PUSH_I64(VALUE)                                                  \
  sp++;                                                                        \
  stack[sp].i64_v = (VALUE);

#define STACK_PUSH_F32(VALUE)                                                  \
  sp++;                                                                        \
  stack[sp].f32_v = (VALUE);

#define STACK_PUSH_F64(VALUE)                                                  \
  sp++;                                                                        \
  stack[sp].f64_v = (VALUE);

#define STACK_PUSH_OBJECT(VALUE)                                               \
  sp++;                                                                        \
  stack[sp].obj_v = (VALUE);                                                   \
  is_gc_object[sp] = 1;

#define STACK_POP_I32(VALUE)                                                   \
  (VALUE) = stack[sp].i32_v;                                                   \
  sp--;

#define STACK_POP_I64(VALUE)                                                   \
  (VALUE) = stack[sp].i64_v;                                                   \
  sp--;

#define STACK_POP_F32(VALUE)                                                   \
  (VALUE) = stack[sp].f32_v;                                                   \
  sp--;

#define STACK_POP_F64(VALUE)                                                   \
  (VALUE) = stack[sp].f64_v;                                                   \
  sp--;

#define STACK_POP_OBJECT(VALUE)                                                \
  (VALUE) = stack[sp].obj_v;                                                   \
  is_gc_object[sp] = 0;                                                        \
  sp--;

#define HEAP_PUT(HEAP, GC_OBJECT)                                              \
  (GC_OBJECT)->next = (HEAP);                                                  \
  (HEAP) = (GC_OBJECT);

#define READ_1BYTE(VALUE)                                                      \
  (VALUE) = code[pc];                                                          \
  pc++;

Machine *create_machine(i32 stack_max_size) {
  Machine *machine;

  machine = malloc(sizeof(Machine));
  machine->stack_max_size = stack_max_size;
  machine->stack = malloc(sizeof(Value) * stack_max_size);
  machine->is_gc_object = malloc(sizeof(int) * stack_max_size);
  machine->heap = NULL;
  machine->env.function = NULL;
  machine->env.module = NULL;
  machine->sp = -1;
  machine->fp = 0;
  machine->pc = 0;
  machine->machine_status = MACHINE_STOPPED;

  return machine;
}

void run_machine(Machine *machine) {
  /* state */
  Byte *code;
  i32 code_length;
  Byte op;
  Value *stack;
  u32 *is_gc_object;
  i32 sp;
  i32 fp;
  i32 pc;

  /* type comparison for binary operations */
  u32 left_type;
  u32 right_type;

  /* temporary storage */
  Array *array;
  i32 length;
  i32 offset;

  stack = machine->stack;
  is_gc_object = machine->is_gc_object;
  code = machine->env.function->code;
  code_length = machine->env.function->code_length;
  sp = machine->sp;
  fp = machine->fp;
  pc = machine->pc;
  machine->machine_status = MACHINE_RUNNING;

  length = 0;
  offset = 0;
  array = NULL;

  /* printf("code length: %d\n", code_length); */

  while (pc < code_length) {

    op = code[pc];
    pc++;
    /* printf("op = %s\n", opcode_info[op][0]); */

    switch (op) {
    case PUSH_I32_0: {
      STACK_PUSH_I32(0);
      break;
    }
    case PUSH_I32_1: {
      STACK_PUSH_I32(1);
      break;
    }
    case PUSH_I32_1BYTE: {
      sp++;
      stack[sp].i32_v = code[pc];
      pc++;
      break;
    }
    case PUSH_I32_2BYTES: {
      sp++;
      stack[sp].i32_v = (code[pc] << 8) + (code[pc + 1]);
      pc = pc + 2;
      break;
    }
    case ADD_I32: {
      stack[sp - 1].i32_v = (stack[sp - 1].i32_v + stack[sp].i32_v);
      sp--;
      break;
    }
    case SUB_I32: {
      stack[sp - 1].i32_v = (stack[sp - 1].i32_v - stack[sp].i32_v);
      sp--;
      break;
    }
    case MUL_I32: {
      stack[sp - 1].i32_v = (stack[sp - 1].i32_v * stack[sp].i32_v);
      sp--;
      break;
    }
    case DIV_I32: {
      stack[sp - 1].i32_v = (stack[sp - 1].i32_v / stack[sp].i32_v);
      sp--;
      break;
    }
    case ADD_I64: {
      stack[sp - 1].i64_v = (stack[sp - 1].i64_v + stack[sp].i64_v);
      sp--;
      break;
    }
    case SUB_I64: {
      stack[sp - 1].i64_v = (stack[sp - 1].i64_v - stack[sp].i64_v);
      sp--;
      break;
    }
    case MUL_I64: {
      stack[sp - 1].i64_v = (stack[sp - 1].i64_v * stack[sp].i64_v);
      sp--;
      break;
    }
    case DIV_I64: {
      stack[sp - 1].i64_v = (stack[sp - 1].i64_v / stack[sp].i64_v);
      sp--;
      break;
    }
    case ADD_F32: {
      stack[sp - 1].f32_v = (stack[sp - 1].f32_v + stack[sp].f32_v);
      sp--;
      break;
    }
    case SUB_F32: {
      stack[sp - 1].f32_v = (stack[sp - 1].f32_v - stack[sp].f32_v);
      sp--;
      break;
    }
    case MUL_F32: {
      stack[sp - 1].f32_v = (stack[sp - 1].f32_v * stack[sp].f32_v);
      sp--;
      break;
    }
    case DIV_F32: {
      stack[sp - 1].f32_v = (stack[sp - 1].f32_v / stack[sp].f32_v);
      sp--;
      break;
    }
    case ADD_F64: {
      stack[sp - 1].f64_v = (stack[sp - 1].f64_v + stack[sp].f64_v);
      sp--;
      break;
    }
    case SUB_F64: {
      stack[sp - 1].f64_v = (stack[sp - 1].f64_v - stack[sp].f64_v);
      sp--;
      break;
    }
    case MUL_F64: {
      stack[sp - 1].f64_v = (stack[sp - 1].f64_v * stack[sp].f64_v);
      sp--;
      break;
    }
    case DIV_F64: {
      stack[sp - 1].f64_v = (stack[sp - 1].f64_v / stack[sp].f64_v);
      sp--;
      break;
    }
    case MINUS_I32: {
      stack[sp].i32_v = -stack[sp].i32_v;
      break;
    }
    case MINUS_I64: {
      stack[sp].i64_v = -stack[sp].i64_v;
      break;
    }
    case MINUS_F32: {
      stack[sp].f32_v = -stack[sp].f32_v;
      break;
    }
    case MINUS_F64: {
      stack[sp].f64_v = -stack[sp].f64_v;
      break;
    }
    case NEW_ARRAY: {
      length = stack[sp].i32_v;
      if (length >= 0) {
        array = malloc(sizeof(Array));
        array->length = length;

        sp++;
        stack[sp].obj_v = malloc(sizeof(GCObject));
        stack[sp].obj_v->u.arr_v = array;
        HEAP_PUT(machine->heap, stack[sp].obj_v);
        is_gc_object[sp] = 1;

        switch (code[pc]) {
        case TYPE_I32: {
          array->u.i32_array = malloc(sizeof(i32) * length);
          stack[sp].obj_v->kind = GCOBJECT_KIND_I32_ARRAY;
          break;
        }
        case TYPE_I64: {
          array->u.i64_array = malloc(sizeof(i64) * length);
          stack[sp].obj_v->kind = GCOBJECT_KIND_I64_ARRAY;
          break;
        }
        case TYPE_F32: {
          array->u.f32_array = malloc(sizeof(f32) * length);
          stack[sp].obj_v->kind = GCOBJECT_KIND_F32_ARRAY;
          break;
        }
        case TYPE_F64: {
          array->u.f64_array = malloc(sizeof(f64) * length);
          stack[sp].obj_v->kind = GCOBJECT_KIND_F64_ARRAY;
          break;
        }
        default: {
          array->u.obj_array = malloc(sizeof(GCObject *) * length);
          stack[sp].obj_v->kind = GCOBJECT_KIND_OBJ_ARRAY;
          break;
        }
        }
        pc++;
        break;
      } else {
        update_machine_state(machine, sp, fp, pc);
        machine->machine_status = RUNTIME_ERROR_ARRAY_LENGTH_LESS_THAN_ZERO;
        return;
      }
    }
    case PUSH_ARRAY_I32: {
      STACK_POP_I32(offset);
      stack[sp].i32_v = stack[sp].obj_v->u.arr_v->u.i32_array[offset];
      is_gc_object[sp] = 0;
      break;
    }
    case PUSH_ARRAY_I64: {
      STACK_POP_I32(offset);
      stack[sp].i64_v = stack[sp].obj_v->u.arr_v->u.i64_array[offset];
      is_gc_object[sp] = 0;
      break;
    }
    case PUSH_ARRAY_F32: {
      STACK_POP_I32(offset);
      stack[sp].f32_v = stack[sp].obj_v->u.arr_v->u.f32_array[offset];
      is_gc_object[sp] = 0;
      break;
    }
    case PUSH_ARRAY_F64: {
      STACK_POP_I32(offset);
      stack[sp].f64_v = stack[sp].obj_v->u.arr_v->u.f64_array[offset];
      is_gc_object[sp] = 0;
      break;
    }
    case PUSH_ARRAY_OBJECT: {
      STACK_POP_I32(offset);
      stack[sp].obj_v = stack[sp].obj_v->u.arr_v->u.obj_array[offset];
      break;
    }
    case POP_ARRAY_I32: {
      offset = stack[sp - 1].i32_v;
      stack[sp - 2].obj_v->u.arr_v->u.i32_array[offset] = stack[sp].i32_v;
      is_gc_object[sp - 2] = 0;
      sp -= 3;
      break;
    }
    case POP_ARRAY_I64: {
      offset = stack[sp - 1].i32_v;
      stack[sp - 2].obj_v->u.arr_v->u.i64_array[offset] = stack[sp].i64_v;
      is_gc_object[sp - 2] = 0;
      sp -= 3;
      break;
    }
    case POP_ARRAY_F32: {
      offset = stack[sp - 1].i32_v;
      stack[sp - 2].obj_v->u.arr_v->u.f32_array[offset] = stack[sp].f32_v;
      is_gc_object[sp - 2] = 0;
      sp -= 3;
      break;
    }
    case POP_ARRAY_F64: {
      offset = stack[sp - 1].i32_v;
      stack[sp - 2].obj_v->u.arr_v->u.f64_array[offset] = stack[sp].f64_v;
      is_gc_object[sp - 2] = 0;
      sp -= 3;
      break;
    }
    case POP_ARRAY_OBJECT: {
      offset = stack[sp - 1].i32_v;
      stack[sp - 2].obj_v->u.arr_v->u.obj_array[offset] = stack[sp].obj_v;
      is_gc_object[sp] = 0;
      is_gc_object[sp - 2] = 0;
      sp -= 3;
      break;
    }
    case CAST_I32_TO_I64: {
      stack[sp].i64_v = stack[sp].i32_v;
      break;
    }
    case PUSH_LOCAL_I32: {
      READ_1BYTE(offset);
      STACK_PUSH_I32(stack[fp + offset].i32_v);
      break;
    }
    case PUSH_LOCAL_I64: {
      READ_1BYTE(offset);
      STACK_PUSH_I64(stack[fp + offset].i64_v);
      break;
    }
    case PUSH_LOCAL_F32: {
      READ_1BYTE(offset);
      STACK_PUSH_F32(stack[fp + offset].f32_v);
      break;
    }
    case PUSH_LOCAL_F64: {
      READ_1BYTE(offset);
      STACK_PUSH_F64(stack[fp + offset].f64_v);
      break;
    }
    case PUSH_LOCAL_OBJECT: {
      READ_1BYTE(offset);
      STACK_PUSH_OBJECT(stack[fp + offset].obj_v);
      break;
    }
    case POP_LOCAL_I32: {
      READ_1BYTE(offset);
      STACK_POP_I32(stack[fp + offset].i32_v);
      break;
    }
    case POP_LOCAL_I64: {
      READ_1BYTE(offset);
      STACK_POP_I64(stack[fp + offset].i64_v);
      break;
    }
    case POP_LOCAL_F32: {
      READ_1BYTE(offset);
      STACK_POP_F32(stack[fp + offset].f32_v);
      break;
    }
    case POP_LOCAL_F64: {
      READ_1BYTE(offset);
      STACK_POP_F64(stack[fp + offset].f64_v);
      break;
    }
    case POP_LOCAL_OBJECT: {
      READ_1BYTE(offset);
      STACK_POP_OBJECT(stack[fp + offset].obj_v);
      break;
    }
    }
  }

  update_machine_state(machine, sp, fp, pc);
  machine->machine_status = MACHINE_COMPLETED;
}

void update_machine_state(Machine *machine, i32 sp, i32 fp, i32 pc) {
  machine->sp = sp;
  machine->fp = fp;
  machine->pc = pc;
}

void free_machine(Machine *machine) {
  GCObject *current;
  GCObject *next;

  current = machine->heap;
  while (current != NULL) {
    next = current->next;
    free_gc_object(current);
    current = next;
  }

  free(machine->stack);
  free(machine->is_gc_object);
  free(machine);
}