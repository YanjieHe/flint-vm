#include "value.h"
#include <stdlib.h>
#include <string.h>

Program *create_program(char *file_name, i32 module_count) {
  Program *program;
  int file_name_len;

  program = malloc(sizeof(Program));
  file_name_len = strlen(file_name);
  program->file_name = malloc(sizeof(char) * (file_name_len + 1));
  strcpy(program->file_name, file_name);
  program->module_count = module_count;
  program->modules = malloc(sizeof(Module) * module_count);
  program->entry_module = 0;
  program->entry_function = 0;

  return program;
}

void free_string(String *str) {
  free(str->characters);
  free(str);
}

char *str_to_c_str(String *str) {
  char *c_str = malloc(sizeof(char) * (str->length + 1));
  strncpy(c_str, str->characters, str->length);
  c_str[str->length] = '\0';
  return c_str;
}