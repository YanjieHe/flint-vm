#include "flint_vm_output.h"
#include <stdio.h>

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
    putchar('\n');

    return 0;
  } else {

    return (-1);
  }
}