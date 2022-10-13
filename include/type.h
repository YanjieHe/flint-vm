#ifndef FLINT_VM_TYPE_H
#define FLINT_VM_TYPE_H

enum Type {
  TYPE_EMPTY = 0,
  TYPE_I32 = 1,
  TYPE_I64 = 2,
  TYPE_F32 = 3,
  TYPE_F64 = 4,
  TYPE_STRING = 5
};

const char* type_to_string(int type);

#endif