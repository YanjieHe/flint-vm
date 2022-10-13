#include "byte_code_loader.h"

#include <stdlib.h>

/*
* @brief create a byte code loader
* @param file_name byte code file
* @return The byte code loader. If the file doesn't exist, returns NULL.
*/
ByteCodeLoader *create_byte_code_loader(char *file_name) {
  FILE *file;
  ByteCodeLoader *loader;
  int file_name_len;

  file = fopen(file_name, "rb");
  if (file) {
    loader = malloc(sizeof(ByteCodeLoader));
    printf("allocate loader memory\n");
    file_name_len = strlen(file_name);
    loader->file_name = malloc(sizeof(char) * (file_name_len + 1));
    strcpy(loader->file_name, file_name);
    printf("loader->file_name\n");
    loader->file = file;
    loader->error_messages = NULL;
    return loader;
  } else {
    return NULL;
  }
}

Byte read_byte(ByteCodeLoader *loader) {
  int peek;

  peek = fgetc(loader->file);
  if (peek != EOF) {
    return (Byte)peek;
  } else {
    utarray_push_back(loader->error_messages, "error occurs when reading byte");
    return 0;
  }
}

u16 read_u16(ByteCodeLoader *loader) {
  u16 result;
  int peek;

  peek = fgetc(loader->file);
  if (peek != EOF) {
    result = (u16)peek;
    peek = fgetc(loader->file);
    if (peek != EOF) {
      result = (result << 8) + ((u16)peek);
      return result;
    } else {
      utarray_push_back(loader->error_messages,
                        "error occurs when reading u16");
      return 0;
    }
  } else {
    utarray_push_back(loader->error_messages, "error occurs when reading u16");
    return 0;
  }
}

i32 read_i32(ByteCodeLoader *loader) {
  i32 result;
  int peek;
  int i;

  result = 0;
  for (i = 0; i < sizeof(i32); i++) {
    peek = fgetc(loader->file);
    if (peek != EOF) {
      result = result + (((i32)peek) << (8 * (4 - 1 - i)));
    } else {
      utarray_push_back(loader->error_messages,
                        "error occurs when reading i32");
      return 0;
    }
  }

  return result;
}

String *read_short_string(ByteCodeLoader *loader) {
  String *result;
  Byte length;
  int i;
  int peek;

  length = read_byte(loader);
  if (loader->error_messages) {
    utarray_push_back(loader->error_messages,
                      "fail to read the length of the string");
    return NULL;
  } else {
    result = malloc(sizeof(String));
    result->characters = malloc(sizeof(char) * length);
    result->length = (i32)length;

    for (i = 0; i < result->length; i++) {
      peek = fgetc(loader->file);
      if (peek != EOF) {
        result->characters[i] = (char)peek;
      } else {
        utarray_push_back(loader->error_messages, "fail to read the string");
        free_string(result);
        return NULL;
      }
    }

    return result;
  }
}

void load_function(ByteCodeLoader *loader, Function *function) {
  String *name;
  u16 code_length;
  int i;
  int peek;

  name = read_short_string(loader);
  if (loader->error_messages) {
    utarray_push_back(loader->error_messages,
                      "fail to read the name of the function");
    return;
  } else {
    code_length = read_u16(loader);
    if (loader->error_messages) {
      utarray_push_back(loader->error_messages,
                        "fail to read the code length of the function");
      free_string(name);
      return;
    } else {
      function->name = name;
      function->code_length = code_length;
      function->code = malloc(sizeof(Byte));
      for (i = 0; i < function->code_length; i++) {
        peek = fgetc(loader->file);
        if (peek != EOF) {
          function->code[i] = (char)peek;
        } else {
          utarray_push_back(loader->error_messages,
                            "fail to read function body code");
          free_string(function->name);
          free(function->code);
          function->name = NULL;
          function->code = NULL;
          return;
        }
      }
      return;
    }
  }
}

void load_module(ByteCodeLoader *loader, Module *module) {
  u16 function_count;
  String *name;
  int i;
  int j;

  name = read_short_string(loader);
  if (loader->error_messages) {
    utarray_push_back(loader->error_messages,
                      "fail to read the name of the module");
    return;
  } else {
    function_count = read_u16(loader);
    if (loader->error_messages) {
      utarray_push_back(loader->error_messages,
                        "fail to read the count of functions in the module");
      free_string(name);
      return;
    } else {
      module->name = name;
      module->function_count = function_count;
      module->functions = malloc(sizeof(Function));

      for (i = 0; i < function_count; i++) {
        load_function(loader, &(module->functions[i]));
        if (loader->error_messages) {
          utarray_push_back(loader->error_messages,
                            "fail to load a function in the module");
          for (j = 0; j < i; j++) {
            free_string(module->functions[j].name);
            free(module->functions[j].code);
            module->functions[i].name = NULL;
            module->functions[i].code = NULL;
          }
          free(module->functions);
          free_string(module->name);
          module->functions = NULL;
          module->name = NULL;
          return;
        }
      }
      return;
    }
  }
}

Program *read_byte_code_file(ByteCodeLoader *loader) {
  Program *program;
  u16 entry_module;
  u16 entry_function;
  u16 module_count;
  int i;
  int j;
  int k;

  entry_module = read_u16(loader);
  if (loader->error_messages) {
    utarray_push_back(loader->error_messages,
                      "fail to read the entry module offset");
    return NULL;
  } else {
    entry_function = read_u16(loader);
    if (loader->error_messages) {
      utarray_push_back(loader->error_messages,
                        "fail to read the entry function offset");
      return NULL;
    } else {
      module_count = read_u16(loader);
      if (loader->error_messages) {
        utarray_push_back(loader->error_messages,
                          "fail to read the count of modules");
        return NULL;
      } else {
        program = create_program(loader->file_name, module_count);
        for (i = 0; i < module_count; i++) {
          load_module(loader, &(program->modules[i]));
          if (loader->error_messages) {
            utarray_push_back(loader->error_messages, "fail to read module");
            for (j = 0; j < i; j++) {
              for (k = 0; k < program->modules[j].function_count; k++) {
                free(program->modules[j].functions[k].code);
                free_string(program->modules[j].functions[k].name);
              }
              free(program->modules[j].functions);
              free_string(program->modules[j].name);
            }
            free(program->modules);
            free(program->file_name);
            return NULL;
          }
        }
      }
    }
    return program;
  }
}

void view_program(Program *program) {
  int i;

  printf("program file name: %s\n", program->file_name);
  printf("# of modules: %d\n", program->module_count);
  for (i = 0; i < program->module_count; i++) {
    view_module(&(program->modules[i]));
  }
}

void view_module(Module *module) {
  int i;
  char *module_name;

  module_name = str_to_c_str(module->name);
  printf("module name: %s\n", module_name);
  free(module_name);
  printf("# of functions: %d\n", module->function_count);
  for (i = 0; i < module->function_count; i++) {
    view_function(&(module->functions[i]));
  }
}

void view_function(Function *function) {
  int i;
  char *function_name;

  function_name = str_to_c_str(function->name);
  printf("function name: %s\n", function_name);
  free(function_name);
  printf("function code length: %d\n", function->code_length);
  printf("function code: { ");
  for (i = 0; i < function->code_length; i++) {
    if (i != 0) {
      printf(", ");
    }
    printf("%d", function->code[i]);
  }
  printf(" }");
  printf("\n");
}