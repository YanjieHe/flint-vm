#include "flint_io.h"
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#define NEW_LINE "\r\n"
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#define NEW_LINE "\n"
#else
#error "Unknown platform."
#endif

extern int flint_io_print(Machine *machine) {
  String *str;

  str = FLINT_GET_STRING_ARG(machine, 0);
  if (fwrite(str->characters, sizeof(char), str->length, stdout) ==
      str->length) {

    FLINT_RETURN_VOID(machine);
  } else {

    return NATIVE_FUNCTION_ERROR;
  }
}

extern int flint_io_print_line(Machine *machine) {
  String *str;

  str = FLINT_GET_STRING_ARG(machine, 0);
  if (fwrite(str->characters, sizeof(char), str->length, stdout) ==
      str->length) {
    printf(NEW_LINE);

    FLINT_RETURN_VOID(machine);
  } else {

    return NATIVE_FUNCTION_ERROR;
  }
}

extern int flint_io_put_int(Machine *machine) {
  i32 value;

  value = FLINT_GET_I32_ARG(machine, 0);
  printf("%d", value);

  FLINT_RETURN_VOID(machine);
}

extern int flint_io_put_int_line(Machine *machine) {
  i32 value;

  value = FLINT_GET_I32_ARG(machine, 0);
  printf("%d" NEW_LINE, value);

  FLINT_RETURN_VOID(machine);
}
