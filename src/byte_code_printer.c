#include "byte_code_printer.h"
#include "opcode.h"
#include "stdlib.h"

ByteCodePrinter *create_byte_code_printer(FILE *output,
                                          BOOLEAN is_verbose_mode) {
  ByteCodePrinter *printer;

  printer = malloc(sizeof(ByteCodePrinter));
  printer->output = output;
  printer->is_verbose_mode = is_verbose_mode;
  printer->error_messages = NULL;

  return printer;
}

void print_byte_code(ByteCodePrinter *printer, Byte *code, size_t code_length) {
  size_t i;
  Byte op;
  const char *op_name;

  i = 0;
  while (i < code_length) {
    op = code[i];
    if (op >= OPCODE_COUNT) {
      add_printing_error(printer,
                         "the operation code is not in the correct range");
      return;
    } else {
      op_name = opcode_info[op][0];
      fprintf(printer->output, "%zu: ", i);
      if (strcmp(opcode_info[op][1], "") == 0) {
        fprintf(printer->output, "%s\n", op_name);
        i++;
      } else if (strcmp(opcode_info[op][1], "b") == 0) {
        fprintf(printer->output, "%s  %d\n", op_name, code[i + 1]);
        i += 2;
      } else if (strcmp(opcode_info[op][1], "u") == 0) {
        fprintf(printer->output, "%s  %d\n", op_name,
                TWO_BYTES_TO_U16(code[i + 1], code[i + 2]));
        i += 3;
      } else if (strcmp(opcode_info[op][1], "s") == 0) {
        fprintf(printer->output, "%s  %d\n", op_name,
                TWO_BYTES_TO_I16(code[i + 1], code[i + 2]));
        i += 3;
      } else {
        add_printing_error(
            printer,
            "the operand information for the operation code is incorrect.");
        return;
      }
    }
  }
}

void add_printing_error(ByteCodePrinter *printer, const char *message) {
  append_error_to_error_list(&(printer->error_messages), message);
}

void free_byte_code_printer(ByteCodePrinter *printer) {
  free_error_list(printer->error_messages);
  if (printer->output != stdout) {
    fclose(printer->output);
  }
  free(printer);
}