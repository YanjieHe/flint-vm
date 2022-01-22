#include "machine.h"
#include <stdlib.h>

Machine *create_machine(i32 stack_max_size) {
  Machine *machine;

  machine = malloc(sizeof(Machine));
  machine->stack_max_size = stack_max_size;
  machine->stack = malloc(sizeof(Value) * stack_max_size);
  machine->sp = -1;
  machine->fp = 0;
  machine->pc = 0;

  return machine;
}