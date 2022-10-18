import pandas as pd
import numpy as np


def expand(df):
    result = []
    for (_, op_name, op_type, with_types) in df.itertuples():
        if with_types != "":
            types = with_types.split(',')
            for type_name in types:
                result.append((op_name + "_" + type_name, op_type))
        else:
            result.append((op_name, op_type))
    return result


def make_enum(opcode_list):
    lines = []
    for (i, (op_name, op_type)) in enumerate(opcode_list):
        lines.append("\t{0} = {1}".format(op_name, i))
    return "enum OpCode {{\n{0}\n}};".format(",\n".join(lines))


def make_opcode_info_array(op_list):
    lines = []
    for (op_name, op_type) in op_list:
        lines.append('\t{{ "{0}", "{1}" }}'.format(op_name, op_type))
    return "static const char* opcode_info [][2] = {{\n{0}\n}};".format(",\n".join(lines))


def header_beginning():
    return "\n".join(["#ifndef FLINT_VM_OP_CODE_H", "#define FLINT_VM_OP_CODE_H", ""])


def header_ending():
    return "\n".join(["#endif /* FLINT_VM_OP_CODE_H */"])


def main():
    df = pd.read_csv("opcode.csv")
    df["op_type"].replace(np.nan, "", inplace=True)
    df["with_types"].replace(np.nan, "", inplace=True)

    op_list = expand(df)
    print(header_beginning())
    print(make_enum(op_list), end="\n\n")
    print(make_opcode_info_array(op_list), end="\n\n")
    print(header_ending())


if __name__ == "__main__":
    main()
