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
      } else if (strcmp(opcode_info[op][1], "b") == 0 ||
                 strcmp(opcode_info[op][1], "cp") == 0) {
        fprintf(printer->output, "%s  %d\n", op_name, code[i + 1]);
        i += 2;
      } else if (strcmp(opcode_info[op][1], "u") == 0) {
        fprintf(printer->output, "%s  %d\n", op_name,
                TWO_BYTES_TO_U16(code[i + 1], code[i + 2]));
        i += 3;
      } else if (strcmp(opcode_info[op][1], "s") == 0) {
        if (op == JUMP || op == JUMP_IF_TRUE || op == JUMP_IF_FALSE) {
          fprintf(printer->output, "%s  %d (target = %ld)\n", op_name,
                  TWO_BYTES_TO_I16(code[i + 1], code[i + 2]),
                  i + 3 + TWO_BYTES_TO_I16(code[i + 1], code[i + 2]));
        } else {
          fprintf(printer->output, "%s  %d\n", op_name,
                  TWO_BYTES_TO_I16(code[i + 1], code[i + 2]));
        }
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

void print_function_info(ByteCodePrinter *printer, Function *function) {
  char *function_name;
  int i;

  function_name = str_to_c_str(function->name);
  printf("function: %s\n", function_name);
  free(function_name);
  printf("stack= %d, ", function->stack);
  printf("args_size = %d, ", function->args_size);
  printf("locals = %d\n", function->locals);

  printf("\n");
  printf("constant pool (size = %d):\n", function->constant_pool_size);
  for (i = 0; i < function->constant_pool_size; i++) {
    printf("#%d = ", i);
    switch (function->constant_pool[i].kind) {
    case CONSTANT_KIND_I32: {
      printf("CONSTANT_KIND_I32");
      printf(" %d", function->constant_pool[i].u.i32_v);
      break;
    }
    case CONSTANT_KIND_I64: {
      printf("CONSTANT_KIND_I64");
      printf(" %ld", function->constant_pool[i].u.i64_v);
      break;
    }
    case CONSTANT_KIND_F32: {
      printf("CONSTANT_KIND_F32");
      printf(" %f", function->constant_pool[i].u.f32_v);
      break;
    }
    case CONSTANT_KIND_F64: {
      printf("CONSTANT_KIND_F64");
      printf(" %f", function->constant_pool[i].u.f64_v);
      break;
    }
    case CONSTANT_KIND_STRING:
      printf("CONSTANT_KIND_STRING");
      break;
    case CONSTANT_KIND_FUNCTION:
      printf("CONSTANT_KIND_FUNCTION");
      break;
    case CONSTANT_KIND_STRUCTURE_META_DATA:
      printf("CONSTANT_KIND_STRUCTURE_META_DATA");
      break;
    case CONSTANT_KIND_GLOBAL_VARIABLE:
      printf("CONSTANT_KIND_GLOBAL_VARIABLE");
      break;
    case CONSTANT_KIND_NATIVE_FUNCTION:
      printf("CONSTANT_KIND_NATIVE_FUNCTION");
      break;
    default:
      printf("Unknown ConstantKind value");
      break;
    }
    printf("\n");
  }
  printf("\n");
  printf("byte code (length = %d): \n", function->code_length);
  print_byte_code(printer, function->code, function->code_length);
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