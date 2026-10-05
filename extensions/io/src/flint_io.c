#include "flint_io.h"
#include <inttypes.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#define NEW_LINE "\r\n"
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#define NEW_LINE "\n"
#else
#error "Unknown platform."
#endif

static int write_new_line(FILE *output) {
  if (output == NULL) {
    return FALSE;
  }

  return fwrite(NEW_LINE, sizeof(char), sizeof(NEW_LINE) - 1, output) ==
         sizeof(NEW_LINE) - 1;
}

static int write_character_as_utf8(u32 character, FILE *output) {
  char encoded[4];
  size_t encoded_length;

  if (output == NULL ||
      encode_utf8_code_point(character, encoded, &encoded_length) !=
          UNICODE_SUCCESS) {
    return FALSE;
  }

  return fwrite(encoded, sizeof(char), encoded_length, output) ==
         encoded_length;
}

static int write_string_as_utf8(const String *str, FILE *output) {
  char buffer[4096];
  UnicodeStatus status;
  size_t buffer_length;
  size_t encoded_length;
  i32 i;

  if (str == NULL || output == NULL || str->length < 0 ||
      (str->length > 0 && str->characters == NULL)) {
    return FALSE;
  }

  buffer_length = 0;
  for (i = 0; i < str->length; i++) {
    if (sizeof(buffer) - buffer_length < 4) {
      if (fwrite(buffer, sizeof(char), buffer_length, output) !=
          buffer_length) {
        return FALSE;
      }
      buffer_length = 0;
    }

    status = encode_utf8_code_point(str->characters[i], buffer + buffer_length,
                                    &encoded_length);
    if (status != UNICODE_SUCCESS) {
      return FALSE;
    }
    buffer_length += encoded_length;
  }

  if (buffer_length > 0 &&
      fwrite(buffer, sizeof(char), buffer_length, output) != buffer_length) {
    return FALSE;
  }

  return TRUE;
}

extern int flint_io_print(Machine *machine) {
  String *str;

  str = FLINT_GET_STRING_ARG(machine, 0);
  if (write_string_as_utf8(str, stdout)) {
    FLINT_RETURN_VOID(machine);
  } else {
    return NATIVE_FUNCTION_ERROR;
  }
}

extern int flint_io_print_line(Machine *machine) {
  String *str;

  str = FLINT_GET_STRING_ARG(machine, 0);
  if (write_string_as_utf8(str, stdout) && write_new_line(stdout)) {
    FLINT_RETURN_VOID(machine);
  } else {
    return NATIVE_FUNCTION_ERROR;
  }
}

extern int flint_io_put_char(Machine *machine) {
  u32 value;

  value = FLINT_GET_CHAR_ARG(machine, 0);
  if (!write_character_as_utf8(value, stdout)) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_put_char_line(Machine *machine) {
  u32 value;

  value = FLINT_GET_CHAR_ARG(machine, 0);
  if (!write_character_as_utf8(value, stdout) || !write_new_line(stdout)) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_put_int(Machine *machine) {
  i32 value;

  value = FLINT_GET_I32_ARG(machine, 0);
  if (fprintf(stdout, "%" PRId32, value) < 0) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_put_int_line(Machine *machine) {
  i32 value;

  value = FLINT_GET_I32_ARG(machine, 0);
  if (fprintf(stdout, "%" PRId32, value) < 0 || !write_new_line(stdout)) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_put_long(Machine *machine) {
  i64 value;

  value = FLINT_GET_I64_ARG(machine, 0);
  if (fprintf(stdout, "%" PRId64, value) < 0) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_put_long_line(Machine *machine) {
  i64 value;

  value = FLINT_GET_I64_ARG(machine, 0);
  if (fprintf(stdout, "%" PRId64, value) < 0 || !write_new_line(stdout)) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_put_float(Machine *machine) {
  f32 value;

  value = FLINT_GET_F32_ARG(machine, 0);
  if (fprintf(stdout, "%.9g", (double)value) < 0) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_put_float_line(Machine *machine) {
  f32 value;

  value = FLINT_GET_F32_ARG(machine, 0);
  if (fprintf(stdout, "%.9g", (double)value) < 0 || !write_new_line(stdout)) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_put_double(Machine *machine) {
  f64 value;

  value = FLINT_GET_F64_ARG(machine, 0);
  if (fprintf(stdout, "%.17g", value) < 0) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_put_double_line(Machine *machine) {
  f64 value;

  value = FLINT_GET_F64_ARG(machine, 0);
  if (fprintf(stdout, "%.17g", value) < 0 || !write_new_line(stdout)) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_put_bool(Machine *machine) {
  BOOLEAN value;

  value = FLINT_GET_BOOL_ARG(machine, 0);
  if (fputs(value ? "true" : "false", stdout) == EOF) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_put_bool_line(Machine *machine) {
  BOOLEAN value;

  value = FLINT_GET_BOOL_ARG(machine, 0);
  if (fputs(value ? "true" : "false", stdout) == EOF ||
      !write_new_line(stdout)) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_new_line(Machine *machine) {
  if (!write_new_line(stdout)) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_flush(Machine *machine) {
  if (fflush(stdout) == EOF) {
    return NATIVE_FUNCTION_ERROR;
  }

  FLINT_RETURN_VOID(machine);
}
