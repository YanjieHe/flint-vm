#include "flint_vm_output.h"
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#define NEW_LINE "\r\n"
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#define NEW_LINE "\n"
#else
#error "Unknown platform."
#endif

extern int FLINT_VM_print(Machine *machine) {
  String *str;

  str = GET_STRING_ARG(machine, 0);
  if (fwrite(str->characters, sizeof(char), str->length, stdout) ==
      str->length) {

    return 0;
  } else {

    return (-1);
  }
}

extern int FLINT_VM_println(Machine *machine) {
  String *str;

  str = GET_STRING_ARG(machine, 0);
  if (fwrite(str->characters, sizeof(char), str->length, stdout) ==
      str->length) {
    printf(NEW_LINE);

    return 0;
  } else {

    return (-1);
  }
}

extern int FLINT_VM_put_int(Machine *machine) {
  i32 value;

  value = GET_I32_ARG(machine, 0);
  printf("%d", value);

  return 0;
}

extern int FLINT_VM_put_int_line(Machine *machine) {
  i32 value;

  value = GET_I32_ARG(machine, 0);
  printf("%d" NEW_LINE, value);

  return 0;
}