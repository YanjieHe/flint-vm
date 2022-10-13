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