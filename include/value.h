#ifndef FLINT_VM_VALUE_H
#define FLINT_VM_VALUE_H

#include <stdint.h>

typedef int8_t i8;
typedef uint8_t u8;
typedef uint8_t Byte;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

typedef i32 BOOLEAN;
#define TRUE 1
#define FALSE 0

struct String;
struct Array;
struct Structure;
struct Function;
struct Closure;
struct StructureMetaData;
struct GlobalVariable;
struct NativeFunction;

typedef union {
  i32 i32_v;
  i64 i64_v;
  f32 f32_v;
  f64 f64_v;
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
    struct Closure* closure_v;
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

typedef enum ConstantKind {
  CONSTANT_KIND_I32 = 0,
  CONSTANT_KIND_I64 = 1,
  CONSTANT_KIND_F32 = 2,
  CONSTANT_KIND_F64 = 3,
  CONSTANT_KIND_STRING = 4,
  CONSTANT_KIND_FUNCTION = 5,
  CONSTANT_KIND_STRUCTURE_META_DATA = 6,
  CONSTANT_KIND_GLOBAL_VARIABLE = 7,
  CONSTANT_KIND_NATIVE_FUNCTION = 8
} ConstantKind;

typedef struct Constant {
  ConstantKind kind;
  union {
    i32 i32_v;
    i64 i64_v;
    f32 f32_v;
    f64 f64_v;
    struct Function *func_v;
    struct StructureMetaData *struct_meta_data;
    struct GCObject *obj_v;
    struct GlobalVariable *global_variable_v;
    struct NativeFunction *native_func_v;
  } u;
} Constant;

typedef struct StructureMetaData {
  String *name;
  String **field_names;
  u16 n_values;
} StructureMetaData;

typedef struct Structure {
  StructureMetaData *meta_data;
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

typedef struct NativeLibrary {
  String *library_path;
  void *library_pointer;
} NativeLibrary;

typedef struct NativeFunction {
  NativeLibrary *library;
  String *func_name;
  void *function_pointer;
  i32 args_size;
} NativeFunction;

typedef struct Function {
  /* function name */
  String *name;

  /* stack space */
  i32 stack;
  i32 locals;
  i32 args_size;

  /* constant pool */
  i32 constant_pool_size;
  Constant *constant_pool;

  /* function body byte code */
  i32 code_length;
  Byte *code;
} Function;

typedef struct Closure {
  Structure *captured_values;
  Function *function;
} Closure;

typedef struct CallInfo {
  Function *caller;
  Byte *caller_pc;
  i32 caller_fp;
} CallInfo;

#define CALL_INFO_ALIGN_SIZE                                                   \
  ((sizeof(CallInfo) / sizeof(Value)) +                                        \
   ((sizeof(CallInfo) % sizeof(Value)) ? 1 : 0))

typedef struct GlobalVariable {
  String *name;
  Value value;
  BOOLEAN is_initialized;
  Function *initializer;
} GlobalVariable;

typedef struct Program {
  /* program byte code file name */
  char *file_name;

  /* global variables */
  i32 global_variable_count;
  GlobalVariable *global_variables;

  /* structures */
  i32 structure_count;
  StructureMetaData *structures_meta_data;

  /* functions */
  i32 function_count;
  Function *functions;

  /* native libraries */
  i32 native_library_count;
  NativeLibrary *native_libraries;

  /* native functions */
  i32 native_function_count;
  NativeFunction *native_functions;

  /* entry */
  Function *entry;
} Program;

Program *create_program(char *file_name, i32 global_variable_count,
                        i32 structure_count, i32 function_count,
                        i32 native_library_count, i32 native_function_count,
                        i32 entry_point);
void free_program(Program *program);
String *make_string(const char *s);
void free_string(String *str);
char *str_to_c_str(String *str);
void free_gc_object(GCObject *gc_object);
void init_function(Function *function);
GCObject *wrap_string_into_gc_object(String *str);

#endif /* VALUE_H */