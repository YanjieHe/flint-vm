#include "value.h"
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

Program *create_program(char *file_name, i32 global_variable_count,
                        i32 structure_count, i32 function_count,
                        i32 native_library_count, i32 native_function_count,
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
    program->structures_meta_data[i].n_values = 0;
    program->structures_meta_data[i].field_names = NULL;
  }

  /* functions */
  program->function_count = function_count;
  program->functions = malloc(sizeof(Function) * function_count);

  for (i = 0; i < program->function_count; i++) {
    init_function(&(program->functions[i]));
  }

  /* native libraries */
  program->native_library_count = native_library_count;
  program->native_libraries =
      native_library_count == 0
          ? NULL
          : malloc(sizeof(NativeLibrary) * native_library_count);

  for (i = 0; i < program->native_library_count; i++) {
    program->native_libraries[i].library_path = NULL;
    program->native_libraries[i].library_pointer = NULL;
  }

  /* native functions */
  program->native_function_count = native_function_count;
  program->native_functions =
      native_function_count == 0
          ? NULL
          : malloc(sizeof(NativeFunction) * native_function_count);

  for (i = 0; i < program->native_function_count; i++) {
    program->native_functions[i].func_name = NULL;
    program->native_functions[i].args_size = 0;
    program->native_functions[i].function_pointer = NULL;
    program->native_functions[i].library = NULL;
  }

  /* entry */
  program->entry = &(program->functions[entry_point]);

  return program;
}

void free_program(Program *program) {
  int i;
  int j;
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
    for (j = 0; j < program->structures_meta_data[i].n_values; j++) {
      free_string(program->structures_meta_data[i].field_names[j]);
    }
    free(program->structures_meta_data[i].field_names);
  }
  free(program->structures_meta_data);

  /* functions */
  for (i = 0; i < program->function_count; i++) {
    function = &(program->functions[i]);
    free(function->constant_pool);
    /* TO DO: free constant pool */
    free_string(function->name);
    free(function->code);
  }
  free(program->functions);

  /* native libraries */
  for (i = 0; i < program->native_library_count; i++) {
    free_string(program->native_libraries[i].library_path);
    dlclose(program->native_libraries[i].library_pointer);
  }
  free(program->native_libraries);

  /* native functions */
  for (i = 0; i < program->native_function_count; i++) {
    free_string(program->native_functions[i].func_name);
  }
  free(program->native_functions);

  /* free program itself */
  free(program);
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

void init_function(Function *function) {
  function->args_size = 0;
  function->code = NULL;
  function->code_length = 0;
  function->constant_pool = NULL;
  function->constant_pool_size = 0;
  function->locals = 0;
  function->name = NULL;
  function->stack = 0;
}

GCObject *wrap_string_into_gc_object(String *str) {
  GCObject *obj;

  obj = malloc(sizeof(GCObject));
  obj->kind = GCOBJECT_KIND_STRING;
  obj->next = NULL;
  obj->u.str_v = str;

  return obj;
}