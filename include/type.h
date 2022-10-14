#ifndef FLINT_VM_TYPE_H
#define FLINT_VM_TYPE_H

enum Type {
  TYPE_I32 = 0,
  TYPE_I64 = 1,
  TYPE_F32 = 2,
  TYPE_F64 = 3,
  TYPE_STRING = 4,
};

static const char *type_name[] = {"Integer32", "Integer64", "Float32",
                                  "Float64", "String"};

#endif