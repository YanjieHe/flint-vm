#ifndef FLINT_VM_BYTE_CODE_LOADER_H
#define FLINT_VM_BYTE_CODE_LOADER_H

#include "value.h"
#include <stdio.h>

#define TWO_BYTES_TO_U16(BYTE1, BYTE2)                                         \
  ((((uint16_t)BYTE1) << 8) + ((uint16_t)BYTE2))

#define TWO_BYTES_TO_I16(BYTE1, BYTE2)                                         \
  ((int16_t)((((uint16_t)BYTE1) << 8) + ((uint16_t)BYTE2)))

typedef struct ErrorList {
  char *message;
  struct ErrorList *next;
} ErrorList;

/* @brief It loads the byte code from a disk file. */
typedef struct ByteCodeLoader {
  char *file_name;
  FILE *file;
  ErrorList *error_messages;
} ByteCodeLoader;

ByteCodeLoader *create_byte_code_loader(char *file_name);
void free_byte_code_loader(ByteCodeLoader *loader);

Byte read_byte(ByteCodeLoader *loader);
Byte *read_bytes(ByteCodeLoader *loader, i32 count);
u16 read_u16(ByteCodeLoader *loader);
i32 read_i32(ByteCodeLoader *loader);
i64 read_i64(ByteCodeLoader *loader);
f32 read_f32(ByteCodeLoader *loader);
f64 read_f64(ByteCodeLoader *loader);
String *read_string(ByteCodeLoader *loader);

void load_function(Program *program, ByteCodeLoader *loader,
                   Function *function);
void load_global_variable(Program *program, ByteCodeLoader *loader,
                          GlobalVariable *global_variable);
void load_structure(Program *program, ByteCodeLoader *loader,
                    StructureMetaData *structure_meta);
void load_native_library(Program *program, ByteCodeLoader *loader,
                         NativeLibrary *native_library);
void load_native_function(Program *program, ByteCodeLoader *loader,
                          NativeFunction *native_function);
Program *read_byte_code_file(ByteCodeLoader *loader);

void view_program(Program *program);
void view_function(Function *function);
void add_loading_error(ByteCodeLoader *loader, const char *message);
void append_error_to_error_list(ErrorList **error_list, const char *message);
void show_errors(ErrorList *error_list);
void free_error_list(ErrorList *error_list);

#endif /* FLINT_VM_BYTE_CODE_LOADER_H */