#include <stdlib.h>
#include <stdio.h>
#include "byte_code_loader.h"
#include "byte_code_printer.h"
#include "machine.h"

#define STACK_MAX_SIZE 10000

void print_help_message();

void print_help_message() {
  printf("Usage: flint-vm <executable path>\n");
  printf("\n");
  printf("Arguments:\n");
  printf("  <executable path>   Path to the executable file to run on the "
         "virtual machine.\n");
  printf("\n");
  printf("Options:\n");
  printf("  -h, --help          Display this help message.\n");
}

int main(int argc, char **argv) {
  ByteCodeLoader *loader;
  ByteCodePrinter *printer;
  char *file_name;
  Program *program;
  Machine *machine;

  int disassemble;
  int i;
  char* function_name;
  i32 exit_code;

  file_name = NULL;
  disassemble = 0;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--disassemble") == 0) {
      disassemble = 1;
    } else {
      if (file_name) {
        printf("Error: Invalid command line arguments.\n");
        print_help_message();
      } else {
        file_name = argv[i];
      }
    }
  }

  if (file_name) {
    loader = create_byte_code_loader(file_name);

    if (loader) {
      program = read_byte_code_file(loader);

      if (disassemble) {
        printer = create_byte_code_printer(stdout, TRUE);
        for (i = 0; i < program->function_count; i++) {
          print_function_info(printer, &(program->functions[i]));
        }

      } else {
        machine = create_machine(STACK_MAX_SIZE);
        load_program(machine, program);
        exit_code = run_machine(machine);

        free_byte_code_loader(loader);
        free_program(program);
        free_machine(machine);

        return exit_code;
      }
    } else {
      printf("Error: The specified file \"%s\" does not exist.\n", file_name);
      return 0;
    }
  } else {
    if (argc != 1) {
      printf("Error: Invalid command line arguments.\n");
      printf("\n");
    }

    print_help_message();
    return 0;
  }
  /*
  ByteCodeLoader *loader;
  Program *program;
  Machine *machine;

  printf("sizeof(String) = %ld\n", sizeof(String));
  printf("sizeof(Value) = %ld\n", sizeof(Value));
  printf("sizeof(Array) = %ld\n", sizeof(Array));

  loader = create_byte_code_loader("../examples/byte_code.bin");
  if (loader) {
    program = read_byte_code_file(loader);
    if (program) {
      printf("program is successfully loaded.\n");
      printf("\n========== view program ==========\n");
      view_program(program);
      printf("========== run machine ==========\n");
      machine = create_machine(STACK_MAX_SIZE);
      machine->env.function = &program->modules[program->entry_module]
                                   .functions[program->entry_function];
      run_machine(machine);
    } else {
      printf("program is not successfully loaded.\n");
    }
  } else {
    printf("fail to load the byte code file\n");
  }
  */
  return 0;
}