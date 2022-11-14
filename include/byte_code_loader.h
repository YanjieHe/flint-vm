#ifndef FLINT_VM_BYTE_CODE_LOADER_H
#define FLINT_VM_BYTE_CODE_LOADER_H

#include "value.h"
#include <stdio.h>

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
Program* read_byte_code_file(ByteCodeLoader *loader);

void view_program(Program *program);
void view_function(Function *function);
void view_byte_code(Byte *code, size_t code_length);
void append_error(ByteCodeLoader *loader, const char *message);
void show_errors(ByteCodeLoader* loader);

#endif /* FLINT_VM_BYTE_CODE_LOADER_H */