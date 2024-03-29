import pandas as pd
import numpy as np
from typing import List
import argparse


class OpInfo:
    def __init__(self, op_name, op_type) -> None:
        self.op_name = op_name
        self.op_type = op_type


def expand(df: pd.DataFrame) -> List[OpInfo]:
    """expand opcode data frame to a list of operation information

    Args:
        df (pd.DataFrame): data frame contains information about operations

    Returns:
        List[OpInfo]: a list of operation information
    """
    result = []
    for (_, op_name, op_type, with_types) in df.itertuples():
        if with_types != "":
            types = with_types.split(',')
            for type_name in types:
                result.append(OpInfo(op_name + "_" + type_name, op_type))
        else:
            result.append(OpInfo(op_name, op_type))
    return result


def make_enum(opcode_list: List[OpInfo]) -> str:
    """create C enumeration definition of operation codes

    Args:
        opcode_list (List[OpInfo]): a list of operation information

    Returns:
        str: generated C code
    """
    lines = []
    for (i, op_info) in enumerate(opcode_list):
        lines.append("\t{0} = {1}".format(op_info.op_name, i))
    return "enum OpCode {{\n{0}\n}};".format(",\n".join(lines))


def make_opcode_info_array(op_list: List[OpInfo]) -> str:
    """create a C array definition of operation code information

    Args:
        op_list (List[OpInfo]): a list of operation information

    Returns:
        str: generated C code
    """
    lines = []
    for op_info in op_list:
        lines.append('\t{{ "{0}", "{1}" }}'.format(
            op_info.op_name, op_info.op_type))
    return "static const char* opcode_info [][2] = {{\n{0}\n}};".format(",\n".join(lines))


def make_python_opcode_definition(op_list: List[OpInfo]) -> str:
    lines = []
    lines.append("class Op:")
    for (i, op_info) in enumerate(op_list):
        lines.append("\t{0} = {1}".format(op_info.op_name, i))
    lines.append("")
    return "\n".join(lines)


def header_beginning() -> str:
    return "\n".join(["#ifndef FLINT_VM_OP_CODE_H", "#define FLINT_VM_OP_CODE_H", ""])


def header_ending() -> str:
    return "\n".join(["#endif /* FLINT_VM_OP_CODE_H */"])


def opcode_count_macro(count: int) -> str:
    return "#define OPCODE_COUNT " + str(count)


def print_generated_c_code(op_list: List[OpInfo]) -> None:
    print(header_beginning())
    print(opcode_count_macro(len(op_list)))
    print()
    print(make_enum(op_list), end="\n\n")
    print(make_opcode_info_array(op_list), end="\n\n")
    print(header_ending())


def load_opcode_data_frame(path: str) -> pd.DataFrame:
    df = pd.read_csv(path)
    df["op_type"].replace(np.nan, "", inplace=True)
    df["with_types"].replace(np.nan, "", inplace=True)

    return df


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('--language', type=str, required=True)
    args = parser.parse_args()

    df = load_opcode_data_frame("opcode.csv")
    op_list = expand(df)

    if args.language == "c":
        print_generated_c_code(op_list)
    elif args.language == "python":
        print(make_python_opcode_definition(op_list))
    else:
        raise Exception(
            "language '{0}' is not supported".format(args.language))


if __name__ == "__main__":
    main()
