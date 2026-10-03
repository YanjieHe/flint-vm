#ifndef FLINT_VM_TEST_INTERFACE_H
#define FLINT_VM_TEST_INTERFACE_H

void test_interface_vtable();
void test_resolve_interface_method_references();
void test_invoke_interface();
void test_interface_dispatches_by_receiver_type();
void test_interface_dispatches_nonzero_method_index();
void test_interface_dispatches_multiple_interfaces();

#endif /* FLINT_VM_TEST_INTERFACE_H */
