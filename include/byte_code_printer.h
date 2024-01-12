#ifndef FLINT_VM_BYTE_CODE_PRINTER_H
#define FLINT_VM_BYTE_CODE_PRINTER_H
#include "value.h"
#include "byte_code_loader.h"

typedef struct ByteCodePrinter {
  FILE *output;
  BOOLEAN is_verbose_mode;
  ErrorList *error_messages;
} ByteCodePrinter;

ByteCodePrinter* create_byte_code_printer(FILE* output, BOOLEAN is_verbose_mode);
void print_byte_code(ByteCodePrinter* printer, Byte *code, size_t code_length);
void print_function_info(ByteCodePrinter* printer, Function* function);
void print_native_function_info(ByteCodePrinter* printer, NativeFunction* native_function);
void print_native_library_info(ByteCodePrinter* printer, NativeLibrary* native_library);
void add_printing_error(ByteCodePrinter* printer, const char* message);
void free_byte_code_printer(ByteCodePrinter* printer);

#endif /* FLINT_VM_BYTE_CODE_PRINTER_H */