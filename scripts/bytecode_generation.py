from op import Op
from typing import List, Any
import os


class U16:
    def __init__(self, value: int) -> None:
        self.value = value

    def to_bytes(self) -> bytes:
        return bytes(bytearray([self.value // 256, self.value % 256]))


class I16:
    def __init__(self, value: int) -> None:
        self.value = value

    def to_bytes(self) -> bytes:
        return self.value.to_bytes(2, byteorder="big", signed=True)


class I32:
    def __init__(self, value: int) -> None:
        self.value = value

    def to_bytes(self) -> bytes:
        return self.value.to_bytes(4, byteorder="big", signed=True)


class I64:
    def __init__(self, value: int) -> None:
        self.value = value

    def to_bytes(self) -> bytes:
        return self.value.to_bytes(8, byteorder="big", signed=True)


class F32:
    def __init__(self, value: float) -> None:
        self.value = value

    def to_bytes(self) -> bytes:
        return self.value.to_bytes(4, byteorder="big", signed=True)


class F64:
    def __init__(self, value: float) -> None:
        self.value = value

    def to_bytes(self) -> bytes:
        return self.value.to_bytes(8, byteorder="big", signed=True)


class String:
    def __init__(self, value: str) -> None:
        self.value = value
        assert len(value) < 65536

    def to_bytes(self) -> bytes:
        return U16(len(self.value)).to_bytes() + bytes(self.value, "utf-8")


class ByteCode:
    def __init__(self) -> None:
        self.byte_list: List[int] = []

    def add(self, item: Any) -> None:
        for b in item.to_bytes():
            self.byte_list.append(b)

    def add_byte(self, b: int) -> None:
        assert b >= 0 and b < 256
        self.byte_list.append(b)


class ConstantKind:
    CONSTANT_KIND_I32 = 0
    CONSTANT_KIND_I64 = 1
    CONSTANT_KIND_F32 = 2
    CONSTANT_KIND_F64 = 3
    CONSTANT_KIND_STRING = 4
    CONSTANT_KIND_FUNCTION = 5
    CONSTANT_KIND_STRUCTURE_META_DATA = 6
    CONSTANT_KIND_GLOBAL_VARIABLE = 7
    CONSTANT_KIND_NATIVE_FUNCTION = 8


class Constant:
    def __init__(self, constant_kind: int, value: Any) -> None:
        self.constant_kind = constant_kind
        self.value = value

    def compile(self, byte_code: ByteCode) -> None:
        byte_code.add_byte(self.constant_kind)
        if self.constant_kind == ConstantKind.CONSTANT_KIND_I32:
            byte_code.add(I32(self.value))
        elif self.constant_kind == ConstantKind.CONSTANT_KIND_I64:
            byte_code.add(I64(self.value))
        elif self.constant_kind == ConstantKind.CONSTANT_KIND_F32:
            byte_code.add(F32(self.value))
        elif self.constant_kind == ConstantKind.CONSTANT_KIND_F64:
            byte_code.add(F64(self.value))
        elif self.constant_kind == ConstantKind.CONSTANT_KIND_STRING:
            byte_code.add(String(self.value))
        elif self.constant_kind == ConstantKind.CONSTANT_KIND_FUNCTION:
            byte_code.add(I32(self.value))
        elif self.constant_kind == ConstantKind.CONSTANT_KIND_GLOBAL_VARIABLE:
            byte_code.add(I32(self.value))
        elif self.constant_kind == ConstantKind.CONSTANT_KIND_STRUCTURE_META_DATA:
            byte_code.add(I32(self.value))
        elif self.constant_kind == ConstantKind.CONSTANT_KIND_NATIVE_FUNCTION:
            byte_code.add(I32(self.value))
        else:
            raise Exception()


class GlobalVariable:
    def __init__(self, name: str, initializer_offset: int) -> None:
        self.name = name
        self.initializer_offset = initializer_offset

    def compile(self, byte_code: ByteCode) -> None:
        byte_code.add(String(self.name))
        byte_code.add(I32(self.initializer_offset))


class StructureMeta:
    def __init__(self, name: str, field_names: List[str]) -> None:
        self.name = name
        self.field_names = field_names

    def compile(self, byte_code: ByteCode) -> None:
        byte_code.add(String(self.name))
        byte_code.add(U16(len(self.field_names)))
        for field_name in self.field_names:
            byte_code.add(String(field_name))


class Function:
    def __init__(self, name: str, stack: int, locals: int, args_size: int, constant_pool: List[Constant], code: List[int]) -> None:
        self.name = name
        self.stack = stack
        self.locals = locals
        self.args_size = args_size
        self.constant_pool = constant_pool
        self.code = code

    def compile(self, byte_code: ByteCode) -> None:
        byte_code.add(String(self.name))
        byte_code.add_byte(self.stack)
        byte_code.add_byte(self.locals)
        byte_code.add_byte(self.args_size)
        byte_code.add_byte(len(self.constant_pool))
        for constant in self.constant_pool:
            constant.compile(byte_code)
        byte_code.add(U16(len(self.code)))
        for b in self.code:
            byte_code.add_byte(b)


class NativeLibrary:
    def __init__(self, library_path: str) -> None:
        self.library_path = library_path

    def compile(self, byte_code: ByteCode) -> None:
        byte_code.add(String(self.library_path))


class NativeFunction:
    def __init__(self, function_name: str, args_size: int, native_library_offset: int) -> None:
        self.function_name = function_name
        self.args_size = args_size
        self.native_library_offset = native_library_offset

    def compile(self, byte_code: ByteCode) -> None:
        byte_code.add(String(self.function_name))
        byte_code.add(I32(self.args_size))
        byte_code.add(I32(self.native_library_offset))


class Program:
    def __init__(self, global_variables: List[GlobalVariable], structures: List[StructureMeta], functions: List[Function], native_libraries: List[NativeLibrary], native_functions: List[NativeFunction], entry_point: int) -> None:
        self.global_variables = global_variables
        self.structures = structures
        self.functions = functions
        self.native_libraries = native_libraries
        self.native_functions = native_functions
        self.entry_point = entry_point

    def compile(self, byte_code: ByteCode) -> None:
        byte_code.add(I32(len(self.global_variables)))
        byte_code.add(I32(len(self.structures)))
        byte_code.add(I32(len(self.functions)))
        byte_code.add(I32(len(self.native_libraries)))
        byte_code.add(I32(len(self.native_functions)))
        byte_code.add(I32(self.entry_point))

        for global_variable in self.global_variables:
            global_variable.compile(byte_code)

        for structure_meta in self.structures:
            structure_meta.compile(byte_code)

        for function in self.functions:
            function.compile(byte_code)

        for native_lib in self.native_libraries:
            native_lib.compile(byte_code)

        for native_function in self.native_functions:
            native_function.compile(byte_code)


def subtract() -> ByteCode:
    code = [
        Op.PUSH_I32_2BYTES, 41, 23,
        Op.PUSH_I32_1BYTE, 55,
        Op.SUB_I32,
        Op.HALT
    ]
    entry = Function("subtract", 0, 0, 0, [], code)
    program = Program([], [], [entry], [], [], 0)
    byte_code = ByteCode()
    program.compile(byte_code)
    return byte_code


def loop() -> ByteCode:
    i64_constant_pool_offset = 0
    local_sum_offset = 1
    local_n_offset = 0
    jump_target = I16(-23).to_bytes()
    code = [
        # n = 100
        Op.PUSH_I64, i64_constant_pool_offset,
        Op.POP_LOCAL_I64, local_n_offset,
        Op.PUSH_LOCAL_I64, local_n_offset,

        # sum = 0
        Op.PUSH_I64_0,
        Op.POP_LOCAL_I64, local_sum_offset,

        # n > 0
        Op.PUSH_LOCAL_I64, local_n_offset,
        Op.PUSH_I64_0,
        Op.GT_I64,
        Op.JUMP_IF_FALSE, 0, 14,

        # sum = sum + n
        Op.PUSH_LOCAL_I64, local_sum_offset,
        Op.PUSH_LOCAL_I64, local_n_offset,
        Op.ADD_I64,
        Op.POP_LOCAL_I64, local_sum_offset,

        # n = n - 1
        Op.PUSH_LOCAL_I64, local_n_offset,
        Op.PUSH_I64_1,
        Op.SUB_I64,
        Op.POP_LOCAL_I64, local_n_offset,

        # jump
        Op.JUMP, jump_target[0], jump_target[1],

        # save the summation for checking
        Op.PUSH_LOCAL_I64, local_sum_offset,

        # exit
        Op.HALT
    ]
    entry = Function("loop", 0, 2, 0, [
        Constant(
            ConstantKind.CONSTANT_KIND_I64, 100)], code)
    program = Program([], [], [entry], [], [], 0)
    byte_code = ByteCode()
    program.compile(byte_code)
    return byte_code


def export(output_path: str, byte_code: ByteCode) -> None:
    with open(output_path, "wb") as binary_file:
        binary_file.write(bytearray(byte_code.byte_list))


def main():
    folder = os.path.join("..", "build", "byte_code")

    export(os.path.join(folder, "subtract"), subtract())
    export(os.path.join(folder, "loop"), loop())


if __name__ == "__main__":
    main()
