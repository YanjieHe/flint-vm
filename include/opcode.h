#ifndef FLINT_VM_OP_CODE_H
#define FLINT_VM_OP_CODE_H

#define OPCODE_COUNT 152

enum OpCode {
  HALT = 0,
  PUSH_I32_0 = 1,
  PUSH_I32_1 = 2,
  PUSH_I32_1BYTE = 3,
  PUSH_I32_2BYTES = 4,
  PUSH_I64_0 = 5,
  PUSH_I64_1 = 6,
  PUSH_F32_0 = 7,
  PUSH_F32_1 = 8,
  PUSH_F64_0 = 9,
  PUSH_F64_1 = 10,
  PUSH_I32 = 11,
  PUSH_I64 = 12,
  PUSH_F32 = 13,
  PUSH_F64 = 14,
  PUSH_STRING = 15,
  PUSH_NULL = 16,
  PUSH_LOCAL_I32 = 17,
  PUSH_LOCAL_I64 = 18,
  PUSH_LOCAL_F32 = 19,
  PUSH_LOCAL_F64 = 20,
  PUSH_LOCAL_OBJECT = 21,
  POP_LOCAL_I32 = 22,
  POP_LOCAL_I64 = 23,
  POP_LOCAL_F32 = 24,
  POP_LOCAL_F64 = 25,
  POP_LOCAL_OBJECT = 26,
  PUSH_GLOBAL_I32 = 27,
  PUSH_GLOBAL_I64 = 28,
  PUSH_GLOBAL_F32 = 29,
  PUSH_GLOBAL_F64 = 30,
  PUSH_GLOBAL_OBJECT = 31,
  POP_GLOBAL_I32 = 32,
  POP_GLOBAL_I64 = 33,
  POP_GLOBAL_F32 = 34,
  POP_GLOBAL_F64 = 35,
  POP_GLOBAL_OBJECT = 36,
  PUSH_ARRAY_I32 = 37,
  PUSH_ARRAY_I64 = 38,
  PUSH_ARRAY_F32 = 39,
  PUSH_ARRAY_F64 = 40,
  PUSH_ARRAY_OBJECT = 41,
  POP_ARRAY_I32 = 42,
  POP_ARRAY_I64 = 43,
  POP_ARRAY_F32 = 44,
  POP_ARRAY_F64 = 45,
  POP_ARRAY_OBJECT = 46,
  PUSH_FIELD_I32 = 47,
  PUSH_FIELD_I64 = 48,
  PUSH_FIELD_F32 = 49,
  PUSH_FIELD_F64 = 50,
  PUSH_FIELD_OBJECT = 51,
  POP_FIELD_I32 = 52,
  POP_FIELD_I64 = 53,
  POP_FIELD_F32 = 54,
  POP_FIELD_F64 = 55,
  POP_FIELD_OBJECT = 56,
  ADD_I32 = 57,
  ADD_I64 = 58,
  ADD_F32 = 59,
  ADD_F64 = 60,
  SUB_I32 = 61,
  SUB_I64 = 62,
  SUB_F32 = 63,
  SUB_F64 = 64,
  MUL_I32 = 65,
  MUL_I64 = 66,
  MUL_F32 = 67,
  MUL_F64 = 68,
  DIV_I32 = 69,
  DIV_I64 = 70,
  DIV_F32 = 71,
  DIV_F64 = 72,
  MOD_I32 = 73,
  MOD_I64 = 74,
  MOD_F32 = 75,
  MOD_F64 = 76,
  BIT_AND_I32 = 77,
  BIT_AND_I64 = 78,
  BIT_OR_I32 = 79,
  BIT_OR_I64 = 80,
  BIT_XOR_I32 = 81,
  BIT_XOR_I64 = 82,
  MINUS_I32 = 83,
  MINUS_I64 = 84,
  MINUS_F32 = 85,
  MINUS_F64 = 86,
  BIT_NOT_I32 = 87,
  BIT_NOT_I64 = 88,
  LOGICAL_NOT = 89,
  CAST_I32_TO_I64 = 90,
  CAST_I32_TO_F32 = 91,
  CAST_I32_TO_F64 = 92,
  CAST_I64_TO_I32 = 93,
  CAST_I64_TO_F32 = 94,
  CAST_I64_TO_F64 = 95,
  CAST_F32_TO_I32 = 96,
  CAST_F32_TO_I64 = 97,
  CAST_F32_TO_F64 = 98,
  CAST_F64_TO_I32 = 99,
  CAST_F64_TO_I64 = 100,
  CAST_F64_TO_F32 = 101,
  UP_CAST = 102,
  DOWN_CAST = 103,
  EQ_I32 = 104,
  EQ_I64 = 105,
  EQ_F32 = 106,
  EQ_F64 = 107,
  NE_I32 = 108,
  NE_I64 = 109,
  NE_F32 = 110,
  NE_F64 = 111,
  GT_I32 = 112,
  GT_I64 = 113,
  GT_F32 = 114,
  GT_F64 = 115,
  LT_I32 = 116,
  LT_I64 = 117,
  LT_F32 = 118,
  LT_F64 = 119,
  GE_I32 = 120,
  GE_I64 = 121,
  GE_F32 = 122,
  GE_F64 = 123,
  LE_I32 = 124,
  LE_I64 = 125,
  LE_F32 = 126,
  LE_F64 = 127,
  POP = 128,
  DUPLICATE = 129,
  JUMP = 130,
  JUMP_IF_TRUE = 131,
  JUMP_IF_FALSE = 132,
  INVOKE_FUNCTION = 133,
  INVOKE_NATIVE_FUNCTION = 134,
  RETURN = 135,
  RETURN_I32 = 136,
  RETURN_I64 = 137,
  RETURN_F32 = 138,
  RETURN_F64 = 139,
  RETURN_OBJECT = 140,
  TAIL_CALL = 141,
  NEW = 142,
  NEW_ARRAY = 143,
  NEW_ARRAY_LITERAL_I32 = 144,
  NEW_ARRAY_LITERAL_I64 = 145,
  NEW_ARRAY_LITERAL_F32 = 146,
  NEW_ARRAY_LITERAL_F64 = 147,
  NEW_CLOSURE = 148,
  INVOKE_CLOSURE = 149,
  ARRAY_LENGTH = 150,
  INSTANCE_OF = 151
};

static const char *opcode_info[][2] = {{"HALT", ""},
                                       {"PUSH_I32_0", ""},
                                       {"PUSH_I32_1", ""},
                                       {"PUSH_I32_1BYTE", "b"},
                                       {"PUSH_I32_2BYTES", "s"},
                                       {"PUSH_I64_0", ""},
                                       {"PUSH_I64_1", ""},
                                       {"PUSH_F32_0", ""},
                                       {"PUSH_F32_1", ""},
                                       {"PUSH_F64_0", ""},
                                       {"PUSH_F64_1", ""},
                                       {"PUSH_I32", "cp"},
                                       {"PUSH_I64", "cp"},
                                       {"PUSH_F32", "cp"},
                                       {"PUSH_F64", "cp"},
                                       {"PUSH_STRING", "cp"},
                                       {"PUSH_NULL", ""},
                                       {"PUSH_LOCAL_I32", "b"},
                                       {"PUSH_LOCAL_I64", "b"},
                                       {"PUSH_LOCAL_F32", "b"},
                                       {"PUSH_LOCAL_F64", "b"},
                                       {"PUSH_LOCAL_OBJECT", "b"},
                                       {"POP_LOCAL_I32", "b"},
                                       {"POP_LOCAL_I64", "b"},
                                       {"POP_LOCAL_F32", "b"},
                                       {"POP_LOCAL_F64", "b"},
                                       {"POP_LOCAL_OBJECT", "b"},
                                       {"PUSH_GLOBAL_I32", "b"},
                                       {"PUSH_GLOBAL_I64", "b"},
                                       {"PUSH_GLOBAL_F32", "b"},
                                       {"PUSH_GLOBAL_F64", "b"},
                                       {"PUSH_GLOBAL_OBJECT", "b"},
                                       {"POP_GLOBAL_I32", "b"},
                                       {"POP_GLOBAL_I64", "b"},
                                       {"POP_GLOBAL_F32", "b"},
                                       {"POP_GLOBAL_F64", "b"},
                                       {"POP_GLOBAL_OBJECT", "b"},
                                       {"PUSH_ARRAY_I32", ""},
                                       {"PUSH_ARRAY_I64", ""},
                                       {"PUSH_ARRAY_F32", ""},
                                       {"PUSH_ARRAY_F64", ""},
                                       {"PUSH_ARRAY_OBJECT", ""},
                                       {"POP_ARRAY_I32", ""},
                                       {"POP_ARRAY_I64", ""},
                                       {"POP_ARRAY_F32", ""},
                                       {"POP_ARRAY_F64", ""},
                                       {"POP_ARRAY_OBJECT", ""},
                                       {"PUSH_FIELD_I32", "b"},
                                       {"PUSH_FIELD_I64", "b"},
                                       {"PUSH_FIELD_F32", "b"},
                                       {"PUSH_FIELD_F64", "b"},
                                       {"PUSH_FIELD_OBJECT", "b"},
                                       {"POP_FIELD_I32", "b"},
                                       {"POP_FIELD_I64", "b"},
                                       {"POP_FIELD_F32", "b"},
                                       {"POP_FIELD_F64", "b"},
                                       {"POP_FIELD_OBJECT", "b"},
                                       {"ADD_I32", ""},
                                       {"ADD_I64", ""},
                                       {"ADD_F32", ""},
                                       {"ADD_F64", ""},
                                       {"SUB_I32", ""},
                                       {"SUB_I64", ""},
                                       {"SUB_F32", ""},
                                       {"SUB_F64", ""},
                                       {"MUL_I32", ""},
                                       {"MUL_I64", ""},
                                       {"MUL_F32", ""},
                                       {"MUL_F64", ""},
                                       {"DIV_I32", ""},
                                       {"DIV_I64", ""},
                                       {"DIV_F32", ""},
                                       {"DIV_F64", ""},
                                       {"MOD_I32", ""},
                                       {"MOD_I64", ""},
                                       {"MOD_F32", ""},
                                       {"MOD_F64", ""},
                                       {"BIT_AND_I32", ""},
                                       {"BIT_AND_I64", ""},
                                       {"BIT_OR_I32", ""},
                                       {"BIT_OR_I64", ""},
                                       {"BIT_XOR_I32", ""},
                                       {"BIT_XOR_I64", ""},
                                       {"MINUS_I32", ""},
                                       {"MINUS_I64", ""},
                                       {"MINUS_F32", ""},
                                       {"MINUS_F64", ""},
                                       {"BIT_NOT_I32", ""},
                                       {"BIT_NOT_I64", ""},
                                       {"LOGICAL_NOT", ""},
                                       {"CAST_I32_TO_I64", ""},
                                       {"CAST_I32_TO_F32", ""},
                                       {"CAST_I32_TO_F64", ""},
                                       {"CAST_I64_TO_I32", ""},
                                       {"CAST_I64_TO_F32", ""},
                                       {"CAST_I64_TO_F64", ""},
                                       {"CAST_F32_TO_I32", ""},
                                       {"CAST_F32_TO_I64", ""},
                                       {"CAST_F32_TO_F64", ""},
                                       {"CAST_F64_TO_I32", ""},
                                       {"CAST_F64_TO_I64", ""},
                                       {"CAST_F64_TO_F32", ""},
                                       {"UP_CAST", "b"},
                                       {"DOWN_CAST", "b"},
                                       {"EQ_I32", ""},
                                       {"EQ_I64", ""},
                                       {"EQ_F32", ""},
                                       {"EQ_F64", ""},
                                       {"NE_I32", ""},
                                       {"NE_I64", ""},
                                       {"NE_F32", ""},
                                       {"NE_F64", ""},
                                       {"GT_I32", ""},
                                       {"GT_I64", ""},
                                       {"GT_F32", ""},
                                       {"GT_F64", ""},
                                       {"LT_I32", ""},
                                       {"LT_I64", ""},
                                       {"LT_F32", ""},
                                       {"LT_F64", ""},
                                       {"GE_I32", ""},
                                       {"GE_I64", ""},
                                       {"GE_F32", ""},
                                       {"GE_F64", ""},
                                       {"LE_I32", ""},
                                       {"LE_I64", ""},
                                       {"LE_F32", ""},
                                       {"LE_F64", ""},
                                       {"POP", ""},
                                       {"DUPLICATE", ""},
                                       {"JUMP", "s"},
                                       {"JUMP_IF_TRUE", "s"},
                                       {"JUMP_IF_FALSE", "s"},
                                       {"INVOKE_FUNCTION", "cp"},
                                       {"INVOKE_NATIVE_FUNCTION", "cp"},
                                       {"RETURN", ""},
                                       {"RETURN_I32", ""},
                                       {"RETURN_I64", ""},
                                       {"RETURN_F32", ""},
                                       {"RETURN_F64", ""},
                                       {"RETURN_OBJECT", ""},
                                       {"TAIL_CALL", "b"},
                                       {"NEW", "cp"},
                                       {"NEW_ARRAY", "b"},
                                       {"NEW_ARRAY_LITERAL_I32", "u"},
                                       {"NEW_ARRAY_LITERAL_I64", "u"},
                                       {"NEW_ARRAY_LITERAL_F32", "u"},
                                       {"NEW_ARRAY_LITERAL_F64", "u"},
                                       {"NEW_CLOSURE", "cp"},
                                       {"INVOKE_CLOSURE", ""},
                                       {"ARRAY_LENGTH", ""},
                                       {"INSTANCE_OF", "b"}};

#endif /* FLINT_VM_OP_CODE_H */
