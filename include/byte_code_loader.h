#ifndef FLINT_VM_BYTE_CODE_LOADER_H
#define FLINT_VM_BYTE_CODE_LOADER_H

#include "utarray.h"
#include "value.h"
#include <stdio.h>


/* @brief It loads the byte code from a disk file. */
typedef struct ByteCodeLoader {
  char *file_name;
  FILE *file;
  UT_array *error_messages;
} ByteCodeLoader;

ByteCodeLoader *create_byte_code_loader(char *file_name);

Byte read_byte(ByteCodeLoader *loader);
u16 read_u16(ByteCodeLoader *loader);
i32 read_i32(ByteCodeLoader *loader);
String *read_short_string(ByteCodeLoader *loader);
void load_function(ByteCodeLoader *loader, Function *function);
void load_module(ByteCodeLoader *loader, Module* module);
Program *read_byte_code_file(ByteCodeLoader *loader);
void view_program(Program* program);
void view_module(Module* module);
void view_function(Function* function);
void view_byte_code(Byte* code, size_t code_length);

#endif /* FLINT_VM_BYTE_CODE_LOADER_H */