#include "byte_code_loader.h"
#include "byte_code_printer.h"
#include "logger.h"
#include "machine.h"
#include <stdio.h>
#include <stdlib.h>

#define STACK_MAX_SIZE 10000

void print_help_message();

void print_help_message() {
  printf("Usage: flint-vm [options] <executable path>\n");
  printf("\n");
  printf("Arguments:\n");
  printf("  <executable path>   Path to the executable file to run on the "
         "virtual machine.\n");
  printf("\n");
  printf("Options:\n");
  printf("  --help              Display this help message.\n");
  printf("  --log-file          Path to the log file where logs will be "
         "written (only available if FLINT_VM_DEBUG_MODE is ON).\n");
  printf("                      If not provided, logging messages will be "
         "printed to the console.\n");
  printf("  --log-level         Set the log level (DEBUG, INFO, WARN, ERROR) "
         "(only available if FLINT_VM_DEBUG_MODE is ON).\n");
  printf("  --disassemble       Output the disassembly of the bytecode instead "
         "of running the program.\n");
  printf("\n");
  printf("Examples:\n");
  printf("  flint-vm my_program.bin --log-file logs.txt --log-level DEBUG\n");
  printf("  flint-vm --disassemble my_program.bin\n");
  printf("\n");
  printf("Note: Logging is only enabled if FLINT_VM_DEBUG_MODE is ON.\n");
  printf("      If no log file is specified, logs will be printed to the "
         "console.\n");
}

int main(int argc, char **argv) {
  ByteCodeLoader *loader;
  ByteCodePrinter *printer;
  char *file_name;
  Program *program;
  Machine *machine;

  int disassemble;
  int i;
  char *function_name;
  i32 exit_code;
  char *log_file;
  LogLevel log_level;

  file_name = NULL;
  disassemble = 0;
  log_file = NULL;
  log_level = LOG_LEVEL_DEBUG;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--disassemble") == 0) {
      disassemble = 1;
    } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      print_help_message();

      return 0;
    } else if (strcmp(argv[i], "--log-file") == 0) {
      if (i + 1 < argc) {
        log_file = argv[i + 1];
        i++;
      } else {
        fprintf(stderr, "Error: Missing file path for '--log-file'. Please "
                        "specify the path to the log file.\n");

        return 1;
      }
    } else if (strcmp(argv[i], "--log-level") == 0) {
      if (i + 1 < argc) {
        if (strcmp(argv[i + 1], "DEBUG") == 0) {
          log_level = LOG_LEVEL_DEBUG;
        } else if (strcmp(argv[i + 1], "INFO") == 0) {
          log_level = LOG_LEVEL_INFO;
        } else if (strcmp(argv[i + 1], "WARN") == 0) {
          log_level = LOG_LEVEL_WARN;
        } else if (strcmp(argv[i + 1], "ERROR") == 0) {
          log_level == LOG_LEVEL_ERROR;
        } else {
          fprintf(
              stderr,
              "Error: Invalid log level for '--log-level'. Please specify one "
              "of the following log levels: DEBUG, INFO, WARN, ERROR.\n");

          return 1;
        }
        i++;
      } else {
        fprintf(stderr,
                "Error: Missing log level for '--log-level'. Please specify a "
                "valid log level (DEBUG, INFO, WARN, ERROR).\n");

        return 1;
      }
    } else {
      if (file_name) {
        printf("Error: Invalid command line arguments.\n");
        print_help_message();

        return 1;
      } else {
        file_name = argv[i];
      }
    }
  }

#ifdef FLINT_VM_DEBUG_MODE
  if (log_init(log_file, log_level) != 0) {
    fprintf(stderr,
            "Logger initialization failed, proceeding without logging.\n");
  }
#endif

  if (file_name) {
    loader = create_byte_code_loader(file_name);

    if (loader) {
      program = read_byte_code_file(loader);

      if (disassemble) {
        printer = create_byte_code_printer(stdout, TRUE);
        for (i = 0; i < program->function_count; i++) {
          print_function_info(printer, &(program->functions[i]));
          printf("\n");
        }

        for (i = 0; i < program->native_library_count; i++) {
          print_native_library_info(printer, &(program->native_libraries[i]));
          printf("\n");
        }

        for (i = 0; i < program->native_function_count; i++) {
          print_native_function_info(printer, &(program->native_functions[i]));
          printf("\n");
        }

      } else {
        machine = create_machine(STACK_MAX_SIZE);
        load_program(machine, program);
        exit_code = run_machine(machine);

        free_byte_code_loader(loader);
        free_program(program);
        free_machine(machine);
        log_close();

        return exit_code;
      }
    } else {
      printf("Error: The specified file \"%s\" does not exist.\n", file_name);
      log_close();

      return 0;
    }
  } else {
    if (argc != 1) {
      printf("Error: Invalid command line arguments.\n");
      printf("\n");
    }

    print_help_message();
    log_close();

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