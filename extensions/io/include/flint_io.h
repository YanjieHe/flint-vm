#ifndef FLINT_IO_H
#define FLINT_IO_H
#include "machine.h"

extern int flint_io_print(Machine *machine);

extern int flint_io_print_line(Machine *machine);

extern int flint_io_put_char(Machine *machine);

extern int flint_io_put_char_line(Machine *machine);

extern int flint_io_put_int(Machine *machine);

extern int flint_io_put_int_line(Machine *machine);

extern int flint_io_put_long(Machine *machine);

extern int flint_io_put_long_line(Machine *machine);

extern int flint_io_put_float(Machine *machine);

extern int flint_io_put_float_line(Machine *machine);

extern int flint_io_put_double(Machine *machine);

extern int flint_io_put_double_line(Machine *machine);

extern int flint_io_put_bool(Machine *machine);

extern int flint_io_put_bool_line(Machine *machine);

extern int flint_io_new_line(Machine *machine);

extern int flint_io_flush(Machine *machine);

#endif /* FLINT_IO_H */
