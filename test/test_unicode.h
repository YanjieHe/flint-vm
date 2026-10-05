#ifndef FLINT_VM_TEST_UNICODE_H
#define FLINT_VM_TEST_UNICODE_H

void test_decode_utf8_code_points();
void test_decode_invalid_utf8();
void test_encode_utf8_code_points();
void test_encode_invalid_code_points();
void test_utf8_utf32_conversion();
void test_unicode_empty_and_invalid_arguments();

#endif /* FLINT_VM_TEST_UNICODE_H */
