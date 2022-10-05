#include "type.h"

const char* type_to_string(int type) {
  switch (type) {
    case TYPE_EMPTY:
      return "TYPE_EMPTY";
    case TYPE_I32:
      return "TYPE_I32";
    case TYPE_I64:
      return "TYPE_I64";
    case TYPE_F32:
      return "TYPE_F32";
    case TYPE_F64:
      return "TYPE_F64";
    case TYPE_STRING:
      return "TYPE_STRING";
    default:
      /* TO DO: throw error */
      return "error";
  }
}