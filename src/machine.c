#include "machine.h"
#include "opcode.h"
#include "type.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define READ_1BYTE_U8(VALUE)                                                   \
  (VALUE) = *pc;                                                               \
  pc++;

#define READ_1BYTE_I8(VALUE)                                                   \
  (VALUE) = (i8)(*pc);                                                         \
  pc++;

#define READ_2BYTES_I16(VALUE)                                                 \
  (VALUE) = ((i16)((((u16)(*pc)) << 8) + ((u16)(*(pc + 1)))));                 \
  pc += 2;

#define SAVE_MACHINE_STATE(MACHINE, SP, FP, PC)                                \
  (MACHINE)->sp = (SP);                                                        \
  (MACHINE)->fp = (FP);                                                        \
  (MACHINE)->pc = (PC);

#define RESTORE_CALLER_ENV(CALL_INFO, MACHINE, FP, PC, CONSTANT_POOL)          \
  (MACHINE)->env.function = (CALL_INFO)->caller;                               \
  (FP) = (CALL_INFO)->caller_fp;                                               \
  (PC) = (CALL_INFO)->caller_pc;                                               \
  (CONSTANT_POOL) = (MACHINE)->env.function->constant_pool;

Machine *create_machine(i32 stack_max_size) {
  Machine *machine;

  machine = malloc(sizeof(Machine));
  machine->stack_max_size = stack_max_size;
  machine->stack = malloc(sizeof(Value) * stack_max_size);
  machine->is_gc_object = malloc(sizeof(u8) * stack_max_size);
  machine->heap = NULL;
  machine->env.function = NULL;
  machine->sp = -1;
  machine->fp = 0;
  machine->pc = NULL;
  machine->machine_status = MACHINE_STOPPED;
  memset(machine->is_gc_object, 0, sizeof(u8) * stack_max_size);

  return machine;
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

void load_program(Machine *machine, Program *program) {
  machine->env.function = program->entry;
  machine->sp += (program->entry->locals + program->entry->args_size);
  machine->pc = program->entry->code;
}

void print_stack(Machine *machine, i32 size) {
  i32 i;

  for (i = 0; i < size; i++) {
    printf("#%d: ", i);
    if (machine->is_gc_object[i]) {
      printf("Object. type id: %d\n", machine->stack[i].obj_v->kind);
    } else {
      printf("\tInt32: %d\n", machine->stack[i].i32_v);
      printf("\tInt64: %ld\n", machine->stack[i].i64_v);
      printf("\tFloat32: %f\n", machine->stack[i].f32_v);
      printf("\tFloat64: %lf\n", machine->stack[i].f64_v);
    }
  }
}

void run_machine(Machine *machine) {
  /* state */
  Byte op;
  Value *stack;
  u8 *is_gc_object;
  i32 sp;
  i32 fp;
  Byte *pc;
  Constant *constant_pool;

  /* temporary storage */
  Array *array;
  i32 length;
  i32 offset;
  Function *callee;
  CallInfo *call_info;
  i32 boolean_value;
  Structure *structure;
  GlobalVariable *global_variable;
  NativeFunction *native_function;
  Closure *closure;
  i32 next_call_args_size;
  i32 base;

  stack = machine->stack;
  is_gc_object = machine->is_gc_object;
  sp = machine->sp;
  fp = machine->fp;
  pc = machine->pc;
  constant_pool = machine->env.function->constant_pool;
  machine->machine_status = MACHINE_RUNNING;

  array = NULL;
  length = 0;
  offset = 0;
  callee = NULL;
  call_info = NULL;
  boolean_value = FALSE;
  structure = NULL;
  next_call_args_size = 0;
  base = 0;

  /* printf("code length: %d\n", code_length); */

  while (1) {

    op = *pc;
    pc++;

    /*
    printf("fp = %d, sp = %d\n", fp, sp);
    print_stack(machine, sp + 1);
    printf("\n");
    printf("op = %s\n", opcode_info[op][0]);
    */

    switch (op) {
    case HALT: {
      SAVE_MACHINE_STATE(machine, sp, fp, pc);
      machine->machine_status = MACHINE_STOPPED;
      return;
    }
    case PUSH_I32_0: {
      STACK_PUSH_I32(0);
      break;
    }
    case PUSH_I32_1: {
      STACK_PUSH_I32(1);
      break;
    }
    case PUSH_I64_0: {
      STACK_PUSH_I64(0);
      break;
    }
    case PUSH_I64_1: {
      STACK_PUSH_I64(1);
      break;
    }
    case PUSH_F32_0: {
      STACK_PUSH_F32(0);
      break;
    }
    case PUSH_F32_1: {
      STACK_PUSH_F32(1);
      break;
    }
    case PUSH_F64_0: {
      STACK_PUSH_F64(0);
      break;
    }
    case PUSH_F64_1: {
      STACK_PUSH_F64(1);
      break;
    }
    case PUSH_I32_1BYTE: {
      sp++;
      READ_1BYTE_I8(stack[sp].i32_v);
      break;
    }
    case PUSH_I32_2BYTES: {
      sp++;
      READ_2BYTES_I16(stack[sp].i32_v);
      break;
    }
    case PUSH_I32: {
      READ_1BYTE_U8(offset);
      STACK_PUSH_I32(constant_pool[offset].u.i32_v);
      break;
    }
    case PUSH_I64: {
      READ_1BYTE_U8(offset);
      STACK_PUSH_I64(constant_pool[offset].u.i64_v);
      break;
    }
    case PUSH_F32: {
      READ_1BYTE_U8(offset);
      STACK_PUSH_F32(constant_pool[offset].u.f32_v);
      break;
    }
    case PUSH_F64: {
      READ_1BYTE_U8(offset);
      STACK_PUSH_F64(constant_pool[offset].u.f64_v);
      break;
    }
    case PUSH_STRING: {
      READ_1BYTE_U8(offset);
      STACK_PUSH_OBJECT(constant_pool[offset].u.obj_v);
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
    case EQ_I32: {
      stack[sp - 1].i32_v = (stack[sp - 1].i32_v == stack[sp].i32_v);
      sp--;
      break;
    }
    case NE_I32: {
      stack[sp - 1].i32_v = (stack[sp - 1].i32_v != stack[sp].i32_v);
      sp--;
      break;
    }
    case GT_I32: {
      stack[sp - 1].i32_v = (stack[sp - 1].i32_v > stack[sp].i32_v);
      sp--;
      break;
    }
    case LT_I32: {
      stack[sp - 1].i32_v = (stack[sp - 1].i32_v < stack[sp].i32_v);
      sp--;
      break;
    }
    case GE_I32: {
      stack[sp - 1].i32_v = (stack[sp - 1].i32_v >= stack[sp].i32_v);
      sp--;
      break;
    }
    case LE_I32: {
      stack[sp - 1].i32_v = (stack[sp - 1].i32_v <= stack[sp].i32_v);
      sp--;
      break;
    }
    case EQ_I64: {
      stack[sp - 1].i32_v = (stack[sp - 1].i64_v == stack[sp].i64_v);
      sp--;
      break;
    }
    case NE_I64: {
      stack[sp - 1].i32_v = (stack[sp - 1].i64_v != stack[sp].i64_v);
      sp--;
      break;
    }
    case GT_I64: {
      stack[sp - 1].i32_v = (stack[sp - 1].i64_v > stack[sp].i64_v);
      sp--;
      break;
    }
    case LT_I64: {
      stack[sp - 1].i32_v = (stack[sp - 1].i64_v < stack[sp].i64_v);
      sp--;
      break;
    }
    case GE_I64: {
      stack[sp - 1].i32_v = (stack[sp - 1].i64_v >= stack[sp].i64_v);
      sp--;
      break;
    }
    case LE_I64: {
      stack[sp - 1].i32_v = (stack[sp - 1].i64_v <= stack[sp].i64_v);
      sp--;
      break;
    }
    case EQ_F32: {
      stack[sp - 1].i32_v = (stack[sp - 1].f32_v == stack[sp].f32_v);
      sp--;
      break;
    }
    case NE_F32: {
      stack[sp - 1].i32_v = (stack[sp - 1].f32_v != stack[sp].f32_v);
      sp--;
      break;
    }
    case GT_F32: {
      stack[sp - 1].i32_v = (stack[sp - 1].f32_v > stack[sp].f32_v);
      sp--;
      break;
    }
    case LT_F32: {
      stack[sp - 1].i32_v = (stack[sp - 1].f32_v < stack[sp].f32_v);
      sp--;
      break;
    }
    case GE_F32: {
      stack[sp - 1].i32_v = (stack[sp - 1].f32_v >= stack[sp].f32_v);
      sp--;
      break;
    }
    case LE_F32: {
      stack[sp - 1].i32_v = (stack[sp - 1].f32_v <= stack[sp].f32_v);
      sp--;
      break;
    }
    case EQ_F64: {
      stack[sp - 1].i32_v = (stack[sp - 1].f64_v == stack[sp].f64_v);
      sp--;
      break;
    }
    case NE_F64: {
      stack[sp - 1].i32_v = (stack[sp - 1].f64_v != stack[sp].f64_v);
      sp--;
      break;
    }
    case GT_F64: {
      stack[sp - 1].i32_v = (stack[sp - 1].f64_v > stack[sp].f64_v);
      sp--;
      break;
    }
    case LT_F64: {
      stack[sp - 1].i32_v = (stack[sp - 1].f64_v < stack[sp].f64_v);
      sp--;
      break;
    }
    case GE_F64: {
      stack[sp - 1].i32_v = (stack[sp - 1].f64_v >= stack[sp].f64_v);
      sp--;
      break;
    }
    case LE_F64: {
      stack[sp - 1].i32_v = (stack[sp - 1].f64_v <= stack[sp].f64_v);
      sp--;
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

        switch (*pc) {
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
        SAVE_MACHINE_STATE(machine, sp, fp, pc);
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
    case CAST_I32_TO_F32: {
      stack[sp].f32_v = stack[sp].i32_v;
      break;
    }
    case CAST_I32_TO_F64: {
      stack[sp].f64_v = stack[sp].i32_v;
      break;
    }
    case CAST_I64_TO_I32: {
      stack[sp].i32_v = stack[sp].i64_v;
      break;
    }
    case CAST_I64_TO_F32: {
      stack[sp].f32_v = stack[sp].i64_v;
      break;
    }
    case CAST_I64_TO_F64: {
      stack[sp].f64_v = stack[sp].i64_v;
      break;
    }
    case CAST_F32_TO_I32: {
      stack[sp].i32_v = stack[sp].f32_v;
      break;
    }
    case CAST_F32_TO_I64: {
      stack[sp].i64_v = stack[sp].f32_v;
      break;
    }
    case CAST_F32_TO_F64: {
      stack[sp].f64_v = stack[sp].f32_v;
      break;
    }
    case CAST_F64_TO_I32: {
      stack[sp].i32_v = stack[sp].f64_v;
      break;
    }
    case CAST_F64_TO_I64: {
      stack[sp].i64_v = stack[sp].f64_v;
      break;
    }
    case CAST_F64_TO_F32: {
      stack[sp].f32_v = stack[sp].f64_v;
      break;
    }
    case PUSH_LOCAL_I32: {
      READ_1BYTE_U8(offset);
      STACK_PUSH_I32(stack[fp + offset].i32_v);
      break;
    }
    case PUSH_LOCAL_I64: {
      READ_1BYTE_U8(offset);
      STACK_PUSH_I64(stack[fp + offset].i64_v);
      break;
    }
    case PUSH_LOCAL_F32: {
      READ_1BYTE_U8(offset);
      STACK_PUSH_F32(stack[fp + offset].f32_v);
      break;
    }
    case PUSH_LOCAL_F64: {
      READ_1BYTE_U8(offset);
      STACK_PUSH_F64(stack[fp + offset].f64_v);
      break;
    }
    case PUSH_LOCAL_OBJECT: {
      READ_1BYTE_U8(offset);
      STACK_PUSH_OBJECT(stack[fp + offset].obj_v);
      break;
    }
    case POP_LOCAL_I32: {
      READ_1BYTE_U8(offset);
      STACK_POP_I32(stack[fp + offset].i32_v);
      break;
    }
    case POP_LOCAL_I64: {
      READ_1BYTE_U8(offset);
      STACK_POP_I64(stack[fp + offset].i64_v);
      break;
    }
    case POP_LOCAL_F32: {
      READ_1BYTE_U8(offset);
      STACK_POP_F32(stack[fp + offset].f32_v);
      break;
    }
    case POP_LOCAL_F64: {
      READ_1BYTE_U8(offset);
      STACK_POP_F64(stack[fp + offset].f64_v);
      break;
    }
    case POP_LOCAL_OBJECT: {
      READ_1BYTE_U8(offset);
      STACK_POP_OBJECT(stack[fp + offset].obj_v);
      break;
    }
    case INVOKE_FUNCTION: {
      READ_1BYTE_U8(offset);
      callee = constant_pool[offset].u.func_v;
      sp = sp + callee->locals;
      sp++;

      call_info = (CallInfo *)&(stack[sp]);
      call_info->caller = machine->env.function;
      call_info->caller_fp = fp;
      call_info->caller_pc = pc;

      sp = sp + CALL_INFO_ALIGN_SIZE;
      machine->env.function = callee;
      pc = callee->code;
      fp = sp - CALL_INFO_ALIGN_SIZE - callee->locals - callee->args_size;
      constant_pool = machine->env.function->constant_pool;
      SAVE_MACHINE_STATE(machine, sp, fp, pc);
      break;
    }
    case INVOKE_NATIVE_FUNCTION: {
      READ_1BYTE_U8(offset);
      native_function = constant_pool[offset].u.native_func_v;
      machine->fp = sp - native_function->args_size + 1;
      machine->sp = sp;
      machine->pc = pc;
      if (((int (*)(Machine *))(native_function->function_pointer))(machine) ==
          -1) {
        machine->machine_status = RUNTIME_ERROR_NATIVE_FUNCTION_ERROR;
        return;
      } else {
        sp = machine->sp;
      }
      break;
    }
    case RETURN: {
      call_info = (CallInfo *)&(stack[fp + machine->env.function->locals +
                                      machine->env.function->args_size]);
      memset(is_gc_object + fp, 0, sp - fp + 1);
      sp = fp;
      RESTORE_CALLER_ENV(call_info, machine, fp, pc, constant_pool);
      SAVE_MACHINE_STATE(machine, sp, fp, pc);
      break;
    }
    case RETURN_I32: {
      call_info = (CallInfo *)&(stack[fp + machine->env.function->locals +
                                      machine->env.function->args_size]);
      stack[fp].i32_v = stack[sp].i32_v;
      /* ignore the value on the top of the stack because it is not a GC object
       */
      memset(is_gc_object + fp, 0, sp - fp);
      sp = fp;
      RESTORE_CALLER_ENV(call_info, machine, fp, pc, constant_pool);
      SAVE_MACHINE_STATE(machine, sp, fp, pc);
      break;
    }
    case RETURN_I64: {
      call_info = (CallInfo *)&(stack[fp + machine->env.function->locals +
                                      machine->env.function->args_size]);
      stack[fp].i64_v = stack[sp].i64_v;
      /* ignore the value on the top of the stack because it is not a GC object
       */
      memset(is_gc_object + fp, 0, sp - fp);
      sp = fp;
      RESTORE_CALLER_ENV(call_info, machine, fp, pc, constant_pool);
      SAVE_MACHINE_STATE(machine, sp, fp, pc);
      break;
    }
    case RETURN_F32: {
      call_info = (CallInfo *)&(stack[fp + machine->env.function->locals +
                                      machine->env.function->args_size]);
      stack[fp].f32_v = stack[sp].f32_v;
      /* ignore the value on the top of the stack because it is not a GC object
       */
      memset(is_gc_object + fp, 0, sp - fp);
      sp = fp;
      RESTORE_CALLER_ENV(call_info, machine, fp, pc, constant_pool);
      SAVE_MACHINE_STATE(machine, sp, fp, pc);
      break;
    }
    case RETURN_F64: {
      call_info = (CallInfo *)&(stack[fp + machine->env.function->locals +
                                      machine->env.function->args_size]);
      stack[fp].f64_v = stack[sp].f64_v;
      /* ignore the value on the top of the stack because it is not a GC object
       */
      memset(is_gc_object + fp, 0, sp - fp);
      sp = fp;
      RESTORE_CALLER_ENV(call_info, machine, fp, pc, constant_pool);
      SAVE_MACHINE_STATE(machine, sp, fp, pc);
      break;
    }
    case RETURN_OBJECT: {
      call_info = (CallInfo *)&(stack[fp + machine->env.function->locals +
                                      machine->env.function->args_size]);
      stack[fp].obj_v = stack[sp].obj_v;
      memset(is_gc_object + fp + 1, 0, sp - fp + 1);
      is_gc_object[fp] = TRUE;
      sp = fp;
      RESTORE_CALLER_ENV(call_info, machine, fp, pc, constant_pool);
      SAVE_MACHINE_STATE(machine, sp, fp, pc);
      break;
    }
    case TAIL_CALL: {
      READ_1BYTE_U8(offset);
      callee = constant_pool[offset].u.func_v;
      call_info = (CallInfo *)&(stack[fp + machine->env.function->locals +
                                      machine->env.function->args_size]);
      machine->env.function = call_info->caller;
      next_call_args_size = callee->args_size;
      base = (sp + 1) - next_call_args_size;
      sp = fp;
      fp = call_info->caller_fp;
      pc = call_info->caller_pc;
      memset(is_gc_object + sp, 0, base - sp);
      for (offset = 0; offset < next_call_args_size; offset++) {
        stack[sp + offset] = stack[base + offset];
        is_gc_object[sp + offset] = is_gc_object[base + offset];
      }
      sp = sp + next_call_args_size - 1;

      /* invoke function */
      sp = sp + callee->locals;
      sp++;

      call_info = (CallInfo *)&(stack[sp]);
      call_info->caller = machine->env.function;
      call_info->caller_fp = fp;
      call_info->caller_pc = pc;

      sp = sp + CALL_INFO_ALIGN_SIZE;
      machine->env.function = callee;
      pc = callee->code;
      fp = sp - CALL_INFO_ALIGN_SIZE - callee->locals - callee->args_size;
      constant_pool = machine->env.function->constant_pool;
      SAVE_MACHINE_STATE(machine, sp, fp, pc);
      break;
    }
    case JUMP: {
      READ_2BYTES_I16(offset);
      pc += offset;
      break;
    }
    case JUMP_IF_TRUE: {
      STACK_POP_I32(boolean_value);
      if (boolean_value) {
        READ_2BYTES_I16(offset);
        pc += offset;
      } else {
        pc += 2;
      }
      break;
    }
    case JUMP_IF_FALSE: {
      STACK_POP_I32(boolean_value);
      if (boolean_value) {
        pc += 2;
      } else {
        READ_2BYTES_I16(offset);
        pc += offset;
      }
      break;
    }
    case NEW: {
      READ_1BYTE_U8(offset);
      structure = malloc(sizeof(Structure));
      structure->meta_data = constant_pool[offset].u.struct_meta_data;
      structure->values =
          malloc(sizeof(Value) * structure->meta_data->n_values);
      sp++;
      stack[sp].obj_v = malloc(sizeof(GCObject));
      stack[sp].obj_v->kind = GCOBJECT_KIND_OBJECT;
      stack[sp].obj_v->u.struct_v = structure;
      HEAP_PUT(machine->heap, stack[sp].obj_v);
      is_gc_object[sp] = TRUE;
      break;
    }
    case PUSH_FIELD_I32: {
      READ_1BYTE_U8(offset);
      stack[sp].i32_v = stack[sp].obj_v->u.struct_v->values[offset].i32_v;
      is_gc_object[sp] = FALSE;
      break;
    }
    case PUSH_FIELD_I64: {
      READ_1BYTE_U8(offset);
      stack[sp].i64_v = stack[sp].obj_v->u.struct_v->values[offset].i64_v;
      is_gc_object[sp] = FALSE;
      break;
    }
    case PUSH_FIELD_F32: {
      READ_1BYTE_U8(offset);
      stack[sp].f32_v = stack[sp].obj_v->u.struct_v->values[offset].f32_v;
      is_gc_object[sp] = FALSE;
      break;
    }
    case PUSH_FIELD_F64: {
      READ_1BYTE_U8(offset);
      stack[sp].f64_v = stack[sp].obj_v->u.struct_v->values[offset].f64_v;
      is_gc_object[sp] = FALSE;
      break;
    }
    case PUSH_FIELD_OBJECT: {
      READ_1BYTE_U8(offset);
      stack[sp].obj_v = stack[sp].obj_v->u.struct_v->values[offset].obj_v;
      break;
    }
    case POP_FIELD_I32: {
      READ_1BYTE_U8(offset);
      stack[sp - 1].obj_v->u.struct_v->values[offset].i32_v = stack[sp].i32_v;
      is_gc_object[sp - 1] = FALSE;
      sp = sp - 2;
      break;
    }
    case POP_FIELD_I64: {
      READ_1BYTE_U8(offset);
      stack[sp - 1].obj_v->u.struct_v->values[offset].i64_v = stack[sp].i64_v;
      is_gc_object[sp - 1] = FALSE;
      sp = sp - 2;
      break;
    }
    case POP_FIELD_F32: {
      READ_1BYTE_U8(offset);
      stack[sp - 1].obj_v->u.struct_v->values[offset].f32_v = stack[sp].f32_v;
      is_gc_object[sp - 1] = FALSE;
      sp = sp - 2;
      break;
    }
    case POP_FIELD_F64: {
      READ_1BYTE_U8(offset);
      stack[sp - 1].obj_v->u.struct_v->values[offset].f64_v = stack[sp].f64_v;
      is_gc_object[sp - 1] = FALSE;
      sp = sp - 2;
      break;
    }
    case POP_FIELD_OBJECT: {
      READ_1BYTE_U8(offset);
      stack[sp - 1].obj_v->u.struct_v->values[offset].obj_v = stack[sp].obj_v;
      is_gc_object[sp - 1] = FALSE;
      is_gc_object[sp] = FALSE;
      sp = sp - 2;
      break;
    }
    case DUPLICATE: {
      sp++;
      stack[sp].obj_v = stack[sp - 1].obj_v;
      is_gc_object[sp] = is_gc_object[sp - 1];
      break;
    }
    case PUSH_GLOBAL_I32: {
      READ_1BYTE_U8(offset);
      global_variable = constant_pool[offset].u.global_variable_v;
      if (global_variable->is_initialized) {
        STACK_PUSH_I32(global_variable->value.i32_v);
      } else {
        callee = global_variable->initializer;
        sp = sp + callee->locals;
        sp++;
        call_info = (CallInfo *)&(stack[sp]);
        call_info->caller = machine->env.function;
        call_info->caller_fp = fp;
        call_info->caller_pc = pc - 2;
        sp = sp + CALL_INFO_ALIGN_SIZE;
        machine->env.function = callee;
        pc = callee->code;
        fp = sp - CALL_INFO_ALIGN_SIZE - callee->locals;
        SAVE_MACHINE_STATE(machine, sp, fp, pc);
      }
      break;
    }
    case PUSH_GLOBAL_I64: {
      READ_1BYTE_U8(offset);
      global_variable = constant_pool[offset].u.global_variable_v;
      if (global_variable->is_initialized) {
        STACK_PUSH_I64(global_variable->value.i64_v);
      } else {
        callee = global_variable->initializer;
        sp = sp + callee->locals;
        sp++;
        call_info = (CallInfo *)&(stack[sp]);
        call_info->caller = machine->env.function;
        call_info->caller_fp = fp;
        call_info->caller_pc = pc - 2;
        sp = sp + CALL_INFO_ALIGN_SIZE;
        machine->env.function = callee;
        pc = callee->code;
        fp = sp - CALL_INFO_ALIGN_SIZE - callee->locals;
        SAVE_MACHINE_STATE(machine, sp, fp, pc);
      }
      break;
    }
    case PUSH_GLOBAL_F32: {
      READ_1BYTE_U8(offset);
      global_variable = constant_pool[offset].u.global_variable_v;
      if (global_variable->is_initialized) {
        STACK_PUSH_F32(global_variable->value.f32_v);
      } else {
        callee = global_variable->initializer;
        sp = sp + callee->locals;
        sp++;
        call_info = (CallInfo *)&(stack[sp]);
        call_info->caller = machine->env.function;
        call_info->caller_fp = fp;
        call_info->caller_pc = pc - 2;
        sp = sp + CALL_INFO_ALIGN_SIZE;
        machine->env.function = callee;
        pc = callee->code;
        fp = sp - CALL_INFO_ALIGN_SIZE - callee->locals;
        SAVE_MACHINE_STATE(machine, sp, fp, pc);
      }
      break;
    }
    case PUSH_GLOBAL_F64: {
      READ_1BYTE_U8(offset);
      global_variable = constant_pool[offset].u.global_variable_v;
      if (global_variable->is_initialized) {
        STACK_PUSH_F64(global_variable->value.f64_v);
      } else {
        callee = global_variable->initializer;
        sp = sp + callee->locals;
        sp++;
        call_info = (CallInfo *)&(stack[sp]);
        call_info->caller = machine->env.function;
        call_info->caller_fp = fp;
        call_info->caller_pc = pc - 2;
        sp = sp + CALL_INFO_ALIGN_SIZE;
        machine->env.function = callee;
        pc = callee->code;
        fp = sp - CALL_INFO_ALIGN_SIZE - callee->locals;
        SAVE_MACHINE_STATE(machine, sp, fp, pc);
      }
      break;
    }
    case PUSH_GLOBAL_OBJECT: {
      READ_1BYTE_U8(offset);
      global_variable = constant_pool[offset].u.global_variable_v;
      if (global_variable->is_initialized) {
        STACK_PUSH_OBJECT(global_variable->value.obj_v);
      } else {
        callee = global_variable->initializer;
        sp = sp + callee->locals;
        sp++;
        call_info = (CallInfo *)&(stack[sp]);
        call_info->caller = machine->env.function;
        call_info->caller_fp = fp;
        call_info->caller_pc = pc - 2;
        sp = sp + CALL_INFO_ALIGN_SIZE;
        machine->env.function = callee;
        pc = callee->code;
        fp = sp - CALL_INFO_ALIGN_SIZE - callee->locals;
        SAVE_MACHINE_STATE(machine, sp, fp, pc);
      }
      break;
    }
    case POP_GLOBAL_I32: {
      READ_1BYTE_U8(offset);
      global_variable = constant_pool[offset].u.global_variable_v;
      STACK_POP_I32(global_variable->value.i32_v);
      global_variable->is_initialized = TRUE;
      break;
    }
    case POP_GLOBAL_I64: {
      READ_1BYTE_U8(offset);
      global_variable = constant_pool[offset].u.global_variable_v;
      STACK_POP_I64(global_variable->value.i64_v);
      global_variable->is_initialized = TRUE;
      break;
    }
    case POP_GLOBAL_F32: {
      READ_1BYTE_U8(offset);
      global_variable = constant_pool[offset].u.global_variable_v;
      STACK_POP_F32(global_variable->value.f32_v);
      global_variable->is_initialized = TRUE;
      break;
    }
    case POP_GLOBAL_F64: {
      READ_1BYTE_U8(offset);
      global_variable = constant_pool[offset].u.global_variable_v;
      STACK_POP_F64(global_variable->value.f64_v);
      global_variable->is_initialized = TRUE;
      break;
    }
    case POP_GLOBAL_OBJECT: {
      READ_1BYTE_U8(offset);
      global_variable = constant_pool[offset].u.global_variable_v;
      STACK_POP_OBJECT(global_variable->value.obj_v);
      global_variable->is_initialized = TRUE;
      break;
    }
    case NEW_CLOSURE: {
      READ_1BYTE_U8(offset);
      closure = malloc(sizeof(Closure));
      closure->captured_values = stack[sp].obj_v;
      closure->function = constant_pool[offset].u.func_v;
      stack[sp].obj_v = malloc(sizeof(GCObject));
      stack[sp].obj_v->kind = GCOBJECT_KIND_CLOSURE;
      stack[sp].obj_v->u.closure_v = closure;
      HEAP_PUT(machine->heap, stack[sp].obj_v);
      break;
    }
    case PREPARE_CLOSURE_CALL: {
      READ_1BYTE_U8(offset);
      closure = stack[sp].obj_v->u.closure_v;
      constant_pool[offset].u.func_v = closure->function;
      stack[sp].obj_v = closure->captured_values;
      break;
    }
    }
  }
}