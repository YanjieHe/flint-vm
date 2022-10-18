#ifndef FLINT_VM_VALUE_H
#define FLINT_VM_VALUE_H

#include "uthash.h"
#include <stdint.h>

typedef uint8_t u8;
typedef uint8_t Byte;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

struct String;
struct Array;
struct Structure;
struct Function;
struct Closure;

typedef union {
  i32 i32_v;
  i64 i64_v;
  f32 f32_v;
  f64 f64_v;
  struct Function *func_v;
  struct GCObject *obj_v;
} Value;

typedef enum GCObjectKind {
  GCOBJECT_KIND_STRING,
  GCOBJECT_KIND_I32_ARRAY,
  GCOBJECT_KIND_I64_ARRAY,
  GCOBJECT_KIND_F32_ARRAY,
  GCOBJECT_KIND_F64_ARRAY,
  GCOBJECT_KIND_OBJ_ARRAY,
  GCOBJECT_KIND_OBJECT
} GCObjectKind;

typedef struct GCObject {
  /* garbage collection information */
  struct GCObject *next;

  GCObjectKind kind;
  union {
    struct String *str_v;
    struct Array *arr_v;
    struct Structure *struct_v;
  } u;
} GCObject;

typedef struct String {
  i32 length;
  char *characters;
} String;

typedef struct Array {
  i32 length;
  union {
    i32 *i32_array;
    i64 *i64_array;
    f32 *f32_array;
    f64 *f64_array;
    GCObject **obj_array;
  } u;
} Array;

typedef struct FieldInfo {
  char *name;
  i32 type;
  u8 is_mutable;
} FieldInfo;

typedef struct FieldIndex {
  char *name;
  u16 id;
  UT_hash_handle hh;
} FieldIndex;

typedef struct StructureInfo {
  String *name;
  FieldIndex *fields_map;
  FieldInfo *fields_arr;
} StructureInfo;

typedef struct Structure {
  StructureInfo *info;
  u16 n_values;
  Value *values;
} Structure;

typedef struct Method {
  Structure *self;

  /* method name */
  String *name;

  /* method body byte code */
  i32 code_length;
  Byte *code;
} Method;

typedef struct Function {
  /* function name */
  String *name;

  /* stack space */
  i32 stack;
  i32 locals;
  i32 args_size;

  /* constant pool */
  i32 constant_pool_size;
  Value* constant_pool;

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

  /* structures */
  i32 structure_count;
  Structure *structures;
} Module;

typedef struct Program {
  /* program byte code file name */
  char *file_name;

  /* modules */
  i32 module_count;
  Module *modules;

  /* structures */
  i32 structure_count;
  StructureInfo *structures;

  /* entry */
  Function* entry;
} Program;

Program *create_program(char *file_name, i32 module_count);
void init_module(Module* module);
String* make_string(const char* s);
void free_string(String *str);
char *str_to_c_str(String *str);
void free_gc_object(GCObject *gc_object);
void free_program(Program* program);

#endif /* VALUE_H */