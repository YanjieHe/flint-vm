#ifndef FLINT_VM_OUTPUT_H
#define FLINT_VM_OUTPUT_H
#include "machine.h"

extern int FLINT_VM_print(Machine* machine);

extern int FLINT_VM_println(Machine* machine);

extern int FLINT_VM_put_int(Machine* machine);

extern int FLINT_VM_put_int_line(Machine* machine);

#endif /* FLINT_VM_OUTPUT_H */