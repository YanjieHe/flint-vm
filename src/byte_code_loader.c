#include "byte_code_loader.h"
#include "load_library.h"
#include "opcode.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define STOP_IF_ANY_LOADING_ERROR(LOADER, MESSAGE)                             \
  if ((LOADER)->error_messages) {                                              \
    add_loading_error(LOADER, (MESSAGE));                                      \
                                                                               \
    return;                                                                    \
  }

#define RETURN_NULL_IF_ANY_LOADING_ERROR(LOADER, MESSAGE)                      \
  if ((LOADER)->error_messages) {                                              \
    add_loading_error(LOADER, (MESSAGE));                                      \
                                                                               \
    return NULL;                                                               \
  }

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

    file_name_len = strlen(file_name);
    loader->file_name = malloc(sizeof(char) * (file_name_len + 1));
    strcpy(loader->file_name, file_name);

    loader->file = file;
    loader->error_messages = NULL;
    loader->native_library_resolver = NULL;

    return loader;
  } else {
    return NULL;
  }
}

void free_byte_code_loader(ByteCodeLoader *loader) {
  fclose(loader->file);
  free(loader->file_name);
  free_error_list(loader->error_messages);
  free(loader);
}

Byte read_byte(ByteCodeLoader *loader) {
  int peek;

  peek = fgetc(loader->file);
  if (peek != EOF) {
    return (Byte)peek;
  } else {
    add_loading_error(loader, "error occurs when reading byte");

    return 0;
  }
}

Byte *read_bytes(ByteCodeLoader *loader, i32 count) {
  Byte *bytes;
  int peek;
  int i;

  bytes = malloc(sizeof(Byte) * count);

  for (i = 0; i < count; i++) {
    peek = fgetc(loader->file);
    if (peek != EOF) {
      bytes[i] = (Byte)peek;
    } else {
      add_loading_error(loader, "error occurs when reading bytes");

      free(bytes);
      return NULL;
    }
  }

  return bytes;
}

u16 read_u16(ByteCodeLoader *loader) {
  u16 result;
  int peek;

  peek = fgetc(loader->file);
  if (peek != EOF) {
    result = (u16)peek;
    peek = fgetc(loader->file);
    if (peek != EOF) {
      result = TWO_BYTES_TO_U16(result, peek);

      return result;
    } else {
      add_loading_error(loader,
                        "error occurs when reading the second byte of u16");

      return 0;
    }
  } else {
    add_loading_error(loader,
                      "error occurs when reading the first byte of u16");

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
      add_loading_error(loader, "error occurs when reading i32");

      return 0;
    }
  }

  return result;
}

i64 read_i64(ByteCodeLoader *loader) {
  i64 result;
  int peek;
  int i;

  result = 0;
  for (i = 0; i < sizeof(i64); i++) {
    peek = fgetc(loader->file);
    if (peek != EOF) {
      result = result + (((i64)peek) << (8 * (8 - 1 - i)));
    } else {
      add_loading_error(loader, "error occurs when reading i64");

      return 0;
    }
  }

  return result;
}

f32 read_f32(ByteCodeLoader *loader) {
  static BOOLEAN bits[sizeof(f32) * 8];
  BOOLEAN *bit_ptr;
  int i;
  int j;
  int sign;
  int exponent;
  Byte *bytes;
  Byte bit;
  f32 mantissa;
  f32 cur;

  bit_ptr = &(bits[0]);
  bytes = read_bytes(loader, sizeof(f32));
  if (loader->error_messages) {
    add_loading_error(loader, "error occurs when reading f32");

    return 0;
  }
  for (i = 0; i < sizeof(f32); i++) {
    bit = bytes[i];
    for (j = 0; j < 8; j++) {
      *(bit_ptr) = bit % 2;
      bit = bit / 2;
      bit_ptr++;
    }
  }

  sign = bits[0] ? (-1) : (+1);
  exponent = 0;
  for (i = 0; i < 8; i++) {
    exponent = exponent + bits[1 + i] * (1 << (8 - 1 - i));
  }
  exponent = exponent - 127;
  mantissa = 1.0;
  cur = 0.5;
  for (i = 0; i < 23; i++) {
    mantissa = mantissa + bits[1 + 8 + i] * cur;
    cur = cur / 2;
  }
  return sign * ldexp(mantissa, exponent);
}

f64 read_f64(ByteCodeLoader *loader) {
  static BOOLEAN bits[sizeof(f64) * 8];
  BOOLEAN *bit_ptr;
  int i;
  int j;
  int sign;
  int exponent;
  Byte *bytes;
  Byte bit;
  f64 mantissa;
  f64 cur;

  bit_ptr = &(bits[0]);
  bytes = read_bytes(loader, sizeof(f64));
  if (loader->error_messages) {
    add_loading_error(loader, "error occurs when reading f64");

    return 0;
  }

  for (i = 0; i < sizeof(f64); i++) {
    bit = bytes[i];
    for (j = 0; j < 8; j++) {
      *(bit_ptr) = bit % 2;
      bit = bit / 2;
      bit_ptr++;
    }
  }

  sign = bits[0] ? (-1) : (+1);
  exponent = 0;
  for (i = 0; i < 11; i++) {
    exponent = exponent + bits[1 + i] * (1 << (11 - 1 - i));
  }
  exponent = exponent - 1023;
  mantissa = 1.0;
  cur = 0.5;
  for (i = 0; i < 52; i++) {
    mantissa = mantissa + bits[1 + 11 + i] * cur;
    cur = cur / 2;
  }
  return sign * ldexp(mantissa, exponent);
}

String *read_string(ByteCodeLoader *loader) {
  UnicodeStatus status;
  String *result;
  char *utf8;
  u16 byte_length;
  size_t bytes_read;

  byte_length = read_u16(loader);
  if (loader->error_messages) {
    add_loading_error(loader, "fail to read the length of the string");

    return NULL;
  } else if (byte_length == 0) {
    status = make_string_from_utf8(NULL, 0, &result);

    if (status != UNICODE_SUCCESS) {
      add_loading_error(loader, "fail to create an empty string");
      return NULL;
    } else {
      return result;
    }
  } else {
    utf8 = malloc(sizeof(char) * byte_length);

    if (utf8 == NULL) {
      add_loading_error(loader, "out of memory while reading a string");
      return NULL;
    } else {
      bytes_read = fread(utf8, sizeof(char), byte_length, loader->file);

      if (bytes_read != byte_length) {
        free(utf8);
        add_loading_error(loader, "fail to read the string");
        return NULL;
      } else {
        status = make_string_from_utf8(utf8, (size_t)byte_length, &result);
        free(utf8);

        if (status != UNICODE_SUCCESS) {
          if (status == UNICODE_INVALID_UTF8) {
            add_loading_error(loader, "the string contains invalid UTF-8");
          } else if (status == UNICODE_TRUNCATED_UTF8) {
            add_loading_error(loader, "the string contains truncated UTF-8");
          } else if (status == UNICODE_OUT_OF_MEMORY) {
            add_loading_error(loader,
                              "out of memory while converting a string");
          } else if (status == UNICODE_LENGTH_OVERFLOW) {
            add_loading_error(loader, "the string is too long");
          } else {
            add_loading_error(loader, "fail to convert the string from UTF-8");
          }
          return NULL;
        } else {
          return result;
        }
      }
    }
  }
}

void load_function(Program *program, ByteCodeLoader *loader,
                   Function *function) {
  String *name;
  u16 code_length;
  int i;
  int peek;

  init_function(function);

  function->name = read_string(loader);
  STOP_IF_ANY_LOADING_ERROR(loader, "fail to read the name of the function");

  function->stack = read_byte(loader);
  STOP_IF_ANY_LOADING_ERROR(loader,
                            "fail to read the stack size of the function");

  function->locals = read_byte(loader);
  STOP_IF_ANY_LOADING_ERROR(loader,
                            "fail to read the local space of the function");

  function->args_size = read_byte(loader);
  STOP_IF_ANY_LOADING_ERROR(loader,
                            "fail to read the argument size of the function");

  function->constant_pool_size = read_byte(loader);
  STOP_IF_ANY_LOADING_ERROR(loader,
                            "fail to read the size of the constant pool");

  function->constant_pool =
      malloc(sizeof(Constant) * function->constant_pool_size);

  for (i = 0; i < function->constant_pool_size; i++) {
    peek = read_byte(loader);
    STOP_IF_ANY_LOADING_ERROR(loader, "fail to read the constant pool kind");

    switch (peek) {
    case CONSTANT_KIND_I32: {
      function->constant_pool[i].kind = CONSTANT_KIND_I32;
      function->constant_pool[i].u.i32_v = read_i32(loader);
      break;
    }
    case CONSTANT_KIND_I64: {
      function->constant_pool[i].kind = CONSTANT_KIND_I64;
      function->constant_pool[i].u.i64_v = read_i64(loader);
      break;
    }
    case CONSTANT_KIND_F32: {
      function->constant_pool[i].kind = CONSTANT_KIND_F32;
      function->constant_pool[i].u.f32_v = read_f32(loader);
      break;
    }
    case CONSTANT_KIND_F64: {
      function->constant_pool[i].kind = CONSTANT_KIND_F64;
      function->constant_pool[i].u.f64_v = read_f64(loader);
      break;
    }
    case CONSTANT_KIND_STRING: {
      function->constant_pool[i].kind = CONSTANT_KIND_STRING;
      function->constant_pool[i].u.obj_v =
          wrap_string_into_gc_object(read_string(loader));
      break;
    }
    case CONSTANT_KIND_FUNCTION: {
      function->constant_pool[i].kind = CONSTANT_KIND_FUNCTION;
      function->constant_pool[i].u.func_v =
          &(program->functions[read_i32(loader)]);
      break;
    }
    case CONSTANT_KIND_GLOBAL_VARIABLE: {
      function->constant_pool[i].kind = CONSTANT_KIND_GLOBAL_VARIABLE;
      function->constant_pool[i].u.global_variable_v =
          &(program->global_variables[read_i32(loader)]);
      break;
    }
    case CONSTANT_KIND_STRUCTURE_META_DATA: {
      function->constant_pool[i].kind = CONSTANT_KIND_STRUCTURE_META_DATA;
      function->constant_pool[i].u.struct_meta_data =
          &(program->structures_meta_data[read_i32(loader)]);
      break;
    }
    case CONSTANT_KIND_NATIVE_FUNCTION: {
      function->constant_pool[i].kind = CONSTANT_KIND_NATIVE_FUNCTION;
      function->constant_pool[i].u.native_func_v =
          &(program->native_functions[read_i32(loader)]);
      break;
    }
    case CONSTANT_KIND_INTERFACE_META_DATA: {
      function->constant_pool[i].kind = CONSTANT_KIND_INTERFACE_META_DATA;
      function->constant_pool[i].u.interface_meta_data =
          &(program->interfaces_meta_data[read_i32(loader)]);
      break;
    }
    case CONSTANT_KIND_INTERFACE_METHOD_REFERENCE: {
      function->constant_pool[i].kind =
          CONSTANT_KIND_INTERFACE_METHOD_REFERENCE;
      function->constant_pool[i].u.interface_method_ref_v =
          malloc(sizeof(InterfaceMethodReference));
      function->constant_pool[i].u.interface_method_ref_v->interface_index =
          read_i32(loader);
      function->constant_pool[i].u.interface_method_ref_v->method_index =
          read_u16(loader);
      function->constant_pool[i].u.interface_method_ref_v->args_size = 0;
      break;
    }
    }
    STOP_IF_ANY_LOADING_ERROR(loader,
                              "error occurs when loading constant pool");
  }

  function->code_length = read_u16(loader);
  STOP_IF_ANY_LOADING_ERROR(loader,
                            "fail to read the code length of the function");

  function->code = read_bytes(loader, function->code_length);
  STOP_IF_ANY_LOADING_ERROR(loader, "fail to read function body code");
}

void load_global_variable(Program *program, ByteCodeLoader *loader,
                          GlobalVariable *global_variable) {
  i32 initializer_offset;

  global_variable->name = read_string(loader);
  STOP_IF_ANY_LOADING_ERROR(loader, "fail to read the global variable name");

  initializer_offset = read_i32(loader);
  STOP_IF_ANY_LOADING_ERROR(loader, "fail to read the initializer offset");

  if (initializer_offset >= 0 && initializer_offset < program->function_count) {
    global_variable->initializer = &(program->functions[initializer_offset]);
    global_variable->is_initialized = FALSE;
  } else {
    add_loading_error(loader, "initializer offset not in the range");

    return;
  }
}

void load_structure(Program *program, ByteCodeLoader *loader,
                    StructureMetaData *structure_meta) {
  int i;
  int j;

  structure_meta->name = read_string(loader);
  STOP_IF_ANY_LOADING_ERROR(loader, "fail to read the name of the structure");

  structure_meta->n_values = read_u16(loader);
  STOP_IF_ANY_LOADING_ERROR(loader,
                            "fail to read the number of structure fields");

  structure_meta->field_names =
      malloc(sizeof(String *) * structure_meta->n_values);
  for (i = 0; i < structure_meta->n_values; i++) {
    structure_meta->field_names[i] = NULL;
  }
  for (i = 0; i < structure_meta->n_values; i++) {
    structure_meta->field_names[i] = read_string(loader);
    STOP_IF_ANY_LOADING_ERROR(loader,
                              "fail to read the name of structure fields");
  }

  structure_meta->vtable_entry_count = read_u16(loader);
  STOP_IF_ANY_LOADING_ERROR(loader,
                            "fail to read the number of vtable entries");

  structure_meta->vtable_entries =
      malloc(sizeof(VTableEntry) * structure_meta->vtable_entry_count);
  for (i = 0; i < structure_meta->vtable_entry_count; i++) {
    structure_meta->vtable_entries[i].interface_index = 0;
    structure_meta->vtable_entries[i].method_count = 0;
    structure_meta->vtable_entries[i].methods = NULL;
  }
  for (i = 0; i < structure_meta->vtable_entry_count; i++) {
    structure_meta->vtable_entries[i].interface_index = read_i32(loader);
    STOP_IF_ANY_LOADING_ERROR(loader,
                              "fail to read the interface index of vtable");

    structure_meta->vtable_entries[i].method_count = read_u16(loader);
    STOP_IF_ANY_LOADING_ERROR(loader,
                              "fail to read the number of vtable methods");

    structure_meta->vtable_entries[i].methods = malloc(
        sizeof(Function *) * structure_meta->vtable_entries[i].method_count);
    for (j = 0; j < structure_meta->vtable_entries[i].method_count; j++) {
      structure_meta->vtable_entries[i].methods[j] =
          &(program->functions[read_i32(loader)]);
      STOP_IF_ANY_LOADING_ERROR(loader,
                                "fail to read the function index of vtable");
    }
  }
}

void load_interface(Program *program, ByteCodeLoader *loader,
                    InterfaceMetaData *interface_meta) {
  int i;

  interface_meta->interface_index = read_i32(loader);
  STOP_IF_ANY_LOADING_ERROR(loader, "fail to read the interface index");

  interface_meta->name = read_string(loader);
  STOP_IF_ANY_LOADING_ERROR(loader, "fail to read the name of the interface");

  interface_meta->method_count = read_u16(loader);
  STOP_IF_ANY_LOADING_ERROR(loader,
                            "fail to read the number of interface methods");

  interface_meta->methods =
      malloc(sizeof(InterfaceMethodMetaData) * interface_meta->method_count);
  for (i = 0; i < interface_meta->method_count; i++) {
    interface_meta->methods[i].name = NULL;
    interface_meta->methods[i].args_size = 0;
  }
  for (i = 0; i < interface_meta->method_count; i++) {
    interface_meta->methods[i].name = read_string(loader);
    STOP_IF_ANY_LOADING_ERROR(loader,
                              "fail to read the name of interface method");

    interface_meta->methods[i].args_size = read_u16(loader);
    STOP_IF_ANY_LOADING_ERROR(
        loader, "fail to read the argument size of interface method");
  }
}

void resolve_interface_method_references(Program *program) {
  int i;
  int j;
  Function *function;
  InterfaceMethodReference *interface_method_ref;

  for (i = 0; i < program->function_count; i++) {
    function = &(program->functions[i]);
    for (j = 0; j < function->constant_pool_size; j++) {
      if (function->constant_pool[j].kind ==
          CONSTANT_KIND_INTERFACE_METHOD_REFERENCE) {
        interface_method_ref =
            function->constant_pool[j].u.interface_method_ref_v;
        interface_method_ref->args_size =
            program->interfaces_meta_data[interface_method_ref->interface_index]
                .methods[interface_method_ref->method_index]
                .args_size;
      }
    }
  }
}

void load_native_library(Program *program, ByteCodeLoader *loader,
                         NativeLibrary *native_library) {
  char *library_name;
  char *lib_path;

  native_library->library_name = read_string(loader);
  STOP_IF_ANY_LOADING_ERROR(loader,
                            "fail to read the name of the native library");

  library_name = str_to_c_str(native_library->library_name);
  lib_path = resolve_native_library_path(loader->native_library_resolver,
                                         library_name);
  if (lib_path == NULL) {
    add_loading_error(loader, "the native library cannot be resolved.");
    free(library_name);
  } else {
    native_library->library_pointer = open_dynamic_library(lib_path);
    if (native_library->library_pointer) {
      free(library_name);
      free(lib_path);
    } else {
      add_loading_error(loader, "the dynamic library cannot be loaded.");
      free(library_name);
      free(lib_path);
    }
  }
}

void load_native_function(Program *program, ByteCodeLoader *loader,
                          NativeFunction *native_function) {
  i32 native_lib_offset;
  char *func_name;

  native_function->func_name = read_string(loader);
  STOP_IF_ANY_LOADING_ERROR(loader,
                            "fail to read the name of the native function");

  native_function->args_size = read_u16(loader);
  STOP_IF_ANY_LOADING_ERROR(
      loader, "fail to read the args size of the native function");

  native_lib_offset = read_i32(loader);
  STOP_IF_ANY_LOADING_ERROR(
      loader, "fail to read the native library offset of the native function");

  native_function->library = &(program->native_libraries[native_lib_offset]);
  func_name = str_to_c_str(native_function->func_name);

  native_function->function_pointer = load_function_from_dynamic_library(
      native_function->library->library_pointer, func_name);
  if (native_function->function_pointer) {
    free(func_name);
  } else {
    add_loading_error(
        loader, "the function from the dynamic library cannot be loaded.");
    free(func_name);
  }
}

Program *read_byte_code_file(ByteCodeLoader *loader) {
  Program *program;
  i32 global_variable_count;
  i32 structure_count;
  i32 function_count;
  i32 native_library_count;
  i32 native_function_count;
  i32 interface_count;
  i32 entry_point;
  int i;

  program = NULL;

  global_variable_count = read_i32(loader);
  RETURN_NULL_IF_ANY_LOADING_ERROR(loader,
                                   "fail to read the global variable count");

  structure_count = read_i32(loader);
  RETURN_NULL_IF_ANY_LOADING_ERROR(loader, "fail to read the structure count");

  function_count = read_i32(loader);
  RETURN_NULL_IF_ANY_LOADING_ERROR(loader, "fail to read the function count");

  native_library_count = read_i32(loader);
  RETURN_NULL_IF_ANY_LOADING_ERROR(loader,
                                   "fail to read the native library count");

  native_function_count = read_i32(loader);
  RETURN_NULL_IF_ANY_LOADING_ERROR(loader,
                                   "fail to read the native function count");

  interface_count = read_i32(loader);
  RETURN_NULL_IF_ANY_LOADING_ERROR(loader, "fail to read the interface count");

  entry_point = read_i32(loader);
  RETURN_NULL_IF_ANY_LOADING_ERROR(loader,
                                   "fail to read the entry function offset");

  program =
      create_program(loader->file_name, global_variable_count, structure_count,
                     function_count, native_library_count,
                     native_function_count, interface_count, entry_point);

  for (i = 0; i < program->global_variable_count; i++) {
    load_global_variable(program, loader, &(program->global_variables[i]));
    RETURN_NULL_IF_ANY_LOADING_ERROR(loader,
                                     "fail to load the global variable");
  }

  for (i = 0; i < program->structure_count; i++) {
    load_structure(program, loader, &(program->structures_meta_data[i]));
    RETURN_NULL_IF_ANY_LOADING_ERROR(loader, "fail to load the structure");
  }

  for (i = 0; i < program->function_count; i++) {
    load_function(program, loader, &(program->functions[i]));
    RETURN_NULL_IF_ANY_LOADING_ERROR(loader, "fail to load the function");
  }

  for (i = 0; i < program->native_library_count; i++) {
    load_native_library(program, loader, &(program->native_libraries[i]));
    RETURN_NULL_IF_ANY_LOADING_ERROR(loader, "fail to load the native library");
  }

  for (i = 0; i < program->native_function_count; i++) {
    load_native_function(program, loader, &(program->native_functions[i]));
    RETURN_NULL_IF_ANY_LOADING_ERROR(loader,
                                     "fail to load the native function");
  }

  for (i = 0; i < program->interface_count; i++) {
    load_interface(program, loader, &(program->interfaces_meta_data[i]));
    RETURN_NULL_IF_ANY_LOADING_ERROR(loader, "fail to load the interfaces");
  }

  resolve_interface_method_references(program);

  return program;
}

void view_program(Program *program) {
  int i;

  printf("program file name: %s\n", program->file_name);
  /* TO DO: view global variables, functions and structures */
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

void add_loading_error(ByteCodeLoader *loader, const char *message) {
  append_error_to_error_list(&(loader->error_messages), message);
}

void append_error_to_error_list(ErrorList **error_list, const char *message) {
  int str_length;
  ErrorList *new_error;

  str_length = strlen(message);
  new_error = malloc(sizeof(ErrorList));
  new_error->message = malloc(sizeof(char) * (str_length + 1));
  strncpy(new_error->message, message, str_length + 1);
  new_error->next = (*error_list);
  (*error_list) = new_error;
}

void show_errors(ErrorList *error_list) {
  ErrorList *current_error;

  current_error = error_list;
  while (current_error != NULL) {
    printf("%s\n", current_error->message);
    current_error = current_error->next;
  }
}

void free_error_list(ErrorList *error_list) {
  ErrorList *current_error;
  ErrorList *next_error;

  current_error = error_list;
  while (current_error != NULL) {
    free(current_error->message);
    next_error = current_error->next;
    free(current_error);
    current_error = next_error;
  }
}

char *make_platform_library_file_name(const char *library_name) {
  const char *prefix;
  const char *suffix;
  size_t prefix_length;
  size_t name_length;
  size_t suffix_length;
  char *result;

#ifdef _WIN32
  prefix = "";
  suffix = ".dll";
#elif defined(__APPLE__)
  prefix = "lib";
  suffix = ".dylib";
#else
  prefix = "lib";
  suffix = ".so";
#endif

  if (library_name == NULL) {
    return NULL;
  }

  prefix_length = strlen(prefix);
  name_length = strlen(library_name);
  suffix_length = strlen(suffix);

  result = malloc(prefix_length + name_length + suffix_length + 1);

  if (result == NULL) {
    return NULL;
  }

  memcpy(result, prefix, prefix_length);
  memcpy(result + prefix_length, library_name, name_length);
  memcpy(result + prefix_length + name_length, suffix, suffix_length + 1);

  return result;
}

char *join_path(const char *directory, const char *file_name) {
  size_t directory_length;
  size_t file_name_length;
  int needs_separator;
  char separator;
  char *result;

#ifdef _WIN32
  separator = '\\';
#else
  separator = '/';
#endif

  if (directory == NULL || file_name == NULL) {
    return NULL;
  }

  directory_length = strlen(directory);
  file_name_length = strlen(file_name);
  needs_separator = 0;

  if (directory_length > 0) {
#ifdef _WIN32
    if (directory[directory_length - 1] != '\\' &&
        directory[directory_length - 1] != '/') {
      needs_separator = 1;
    }
#else
    if (directory[directory_length - 1] != '/') {
      needs_separator = 1;
    }
#endif
  }

  result = malloc(directory_length + needs_separator + file_name_length + 1);

  if (result == NULL) {
    return NULL;
  }

  memcpy(result, directory, directory_length);

  if (needs_separator) {
    result[directory_length] = separator;
  }

  memcpy(result + directory_length + needs_separator, file_name,
         file_name_length + 1);

  return result;
}

int file_exists(const char *path) {
  FILE *file;

  if (path == NULL) {
    return 0;
  }

  file = fopen(path, "rb");

  if (file == NULL) {
    return 0;
  }

  fclose(file);
  return 1;
}

char *resolve_native_library_path(const NativeLibraryResolver *resolver,
                                  const char *library_name) {
  char *file_name;
  char *candidate;

  file_name = make_platform_library_file_name(library_name);

  while (resolver) {
    if (resolver->search_path) {
      candidate = join_path(resolver->search_path, file_name);

      if (file_exists(candidate)) {
        free(file_name);

        return candidate;
      } else {
        free(candidate);
      }
    }
    resolver = resolver->next;
  }

  free(file_name);

  return NULL;
}