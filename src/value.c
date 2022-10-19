#include "value.h"
#include <stdlib.h>
#include <string.h>

Program *create_program(char *file_name, i32 module_count) {
  Program *program;
  int file_name_len;
  i32 i;

  program = malloc(sizeof(Program));
  file_name_len = strlen(file_name);
  program->file_name = malloc(sizeof(char) * (file_name_len + 1));
  strcpy(program->file_name, file_name);
  program->module_count = module_count;
  program->modules = malloc(sizeof(Module) * module_count);
  program->entry = NULL;

  for (i = 0; i < module_count; i++) {
    init_module(&(program->modules[i]));
  }

  return program;
}

void init_module(Module *module) {
  module->constant_pool = NULL;
  module->functions = NULL;
  module->name = NULL;
  module->structures = NULL;
}

String *make_string(const char *s) {
  String *str;

  str = malloc(sizeof(String));
  str->length = strlen(s);
  str->characters = malloc(sizeof(char) * str->length);
  memcpy(str->characters, s, str->length);

  return str;
}

void free_string(String *str) {
  if (str != NULL) {
    free(str->characters);
    free(str);
  }
}

char *str_to_c_str(String *str) {
  char *c_str = malloc(sizeof(char) * (str->length + 1));
  strncpy(c_str, str->characters, str->length);
  c_str[str->length] = '\0';

  return c_str;
}

void free_gc_object(GCObject *gc_object) {
  if (gc_object->kind == GCOBJECT_KIND_STRING) {
    free_string(gc_object->u.str_v);
  } else if (gc_object->kind == GCOBJECT_KIND_I32_ARRAY) {
    free(gc_object->u.arr_v->u.i32_array);
    free(gc_object->u.arr_v);
  } else if (gc_object->kind == GCOBJECT_KIND_I64_ARRAY) {
    free(gc_object->u.arr_v->u.i64_array);
    free(gc_object->u.arr_v);
  } else if (gc_object->kind == GCOBJECT_KIND_F32_ARRAY) {
    free(gc_object->u.arr_v->u.f32_array);
    free(gc_object->u.arr_v);
  } else if (gc_object->kind == GCOBJECT_KIND_F64_ARRAY) {
    free(gc_object->u.arr_v->u.f64_array);
    free(gc_object->u.arr_v);
  } else if (gc_object->kind == GCOBJECT_KIND_OBJ_ARRAY) {
    free(gc_object->u.arr_v->u.obj_array);
    free(gc_object->u.arr_v);
  } else {
    /* TO DO: free structure */
  }
  free(gc_object);
}

void free_program(Program *program) {
  i32 i, j;
  Module *module;
  Function *function;

  free(program->file_name);
  for (i = 0; i < program->module_count; i++) {
    module = &(program->modules[i]);
    free_string(module->name);
    free(module->constant_pool);
    for (j = 0; j < module->function_count; j++) {
      function = &(module->functions[j]);
      free(function->constant_pool);
      free_string(function->name);
      free(function->code);
    }
    free(module->functions);
  }
  free(program->modules);
  free(program);
}