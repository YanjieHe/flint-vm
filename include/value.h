#ifndef FLINT_VM_VALUE_H
#define FLINT_VM_VALUE_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint8_t Byte;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

typedef struct Value {
  u32 type;
  union {
    i32 i32_v;
    i64 i64_v;
    f32 f32_v;
    f64 f64_v;
    void *ptr;
  } u;
} Value;

typedef struct String {
  i32 length;
  char *characters;
} String;

typedef struct Function {
  /* function name */
  String *name;

  /* function body byte code */
  i32 code_length;
  Byte *code;
} Function;

typedef struct Module {
  /* module name */
  String *name;

  /* constant pool */
  i32 constant_pool_size;
  Value *constant_pool;

  /* functions */
  i32 function_count;
  Function *functions;
} Module;

typedef struct Program {
  /* program byte code file name */
  char *file_name;

  /* modules */
  i32 module_count;
  Module *modules;
} Program;

Program *create_program(char *file_name, i32 module_count);
void free_string(String *str);
char* str_to_c_str(String* str);

#endif