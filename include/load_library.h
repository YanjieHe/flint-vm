#ifndef FLINT_VM_LOAD_LIBRARY_H
#define FLINT_VM_LOAD_LIBRARY_H

void *open_dynamic_library(const char *library_path);

void *load_function_from_dynamic_library(void *library_pointer,
                                         const char *function_name);

void close_dynamic_library(void *library_pointer);

#endif /* FLINT_VM_LOAD_LIBRARY_H */