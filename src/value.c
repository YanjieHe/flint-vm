#include "value.h"
#include <stdlib.h>
#include <string.h>

Program *create_program(char *file_name, i32 global_variable_count,
                        i32 structure_count, i32 function_count,
                        i32 entry_point) {
  Program *program;
  int file_name_len;
  int i;

  program = malloc(sizeof(Program));

  /* program byte code file name */
  file_name_len = strlen(file_name);
  program->file_name = malloc(sizeof(char) * (file_name_len + 1));
  strcpy(program->file_name, file_name);

  /* global variables */
  program->global_variable_count = global_variable_count;
  program->global_variables =
      global_variable_count == 0
          ? NULL
          : malloc(sizeof(GlobalVariable) * global_variable_count);

  for (i = 0; i < program->global_variable_count; i++) {
    program->global_variables[i].initializer = NULL;
    program->global_variables[i].is_initialized = FALSE;
    program->global_variables[i].name = NULL;
  }

  /* structures */
  program->structure_count = structure_count;
  program->structures_meta_data =
      structure_count == 0
          ? NULL
          : malloc(sizeof(StructureMetaData) * structure_count);

  for (i = 0; i < program->structure_count; i++) {
    program->structures_meta_data[i].name = NULL;
  }

  /* functions */
  program->function_count = function_count;
  program->functions = malloc(sizeof(Function) * function_count);

  for (i = 0; i < program->function_count; i++) {
    program->functions[i].code = NULL;
    program->functions[i].constant_pool = NULL;
  }

  /* entry */
  program->entry = &(program->functions[entry_point]);

  return program;
}

void free_program(Program *program) {
  int i;
  Function *function;

  free(program->file_name);

  /* global variables */
  for (i = 0; i < program->global_variable_count; i++) {
    free_string(program->global_variables[i].name);
  }
  free(program->global_variables);

  /* structures */
  for (i = 0; i < program->structure_count; i++) {
    free_string(program->structures_meta_data[i].name);
  }
  free(program->structures_meta_data);

  /* functions */
  for (i = 0; i < program->function_count; i++) {
    function = &(program->functions[i]);
    free(function->constant_pool);
    free_string(function->name);
    free(function->code);
  }
  free(program->functions);

  /* free program itself */
  free(program);
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
  strncpy(str->characters, s, str->length);

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
  } else if (gc_object->kind == GCOBJECT_KIND_STRING) {
    free_string(gc_object->u.str_v);
  } else {
    free(gc_object->u.struct_v->values);
    free(gc_object->u.struct_v);
  }
  free(gc_object);
}