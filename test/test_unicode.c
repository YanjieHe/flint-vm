#include "test_unicode.h"

#include "test.h"
#include "unicode.h"

void test_decode_utf8_code_points() {
  const char input[] = {'A',
                        (char)0xC2,
                        (char)0xA2,
                        (char)0xE4,
                        (char)0xB8,
                        (char)0xAD,
                        (char)0xF0,
                        (char)0x9F,
                        (char)0x98,
                        (char)0x80};
  size_t offset;
  uint32_t code_point;

  offset = 0;

  ASSERT_EQUAL(decode_next_utf8_code_point(input, sizeof(input), &offset,
                                           &code_point),
               UNICODE_SUCCESS);
  ASSERT_EQUAL(code_point, 0x41u);
  ASSERT_EQUAL(offset, 1);

  ASSERT_EQUAL(decode_next_utf8_code_point(input, sizeof(input), &offset,
                                           &code_point),
               UNICODE_SUCCESS);
  ASSERT_EQUAL(code_point, 0xA2u);
  ASSERT_EQUAL(offset, 3);

  ASSERT_EQUAL(decode_next_utf8_code_point(input, sizeof(input), &offset,
                                           &code_point),
               UNICODE_SUCCESS);
  ASSERT_EQUAL(code_point, 0x4E2Du);
  ASSERT_EQUAL(offset, 6);

  ASSERT_EQUAL(decode_next_utf8_code_point(input, sizeof(input), &offset,
                                           &code_point),
               UNICODE_SUCCESS);
  ASSERT_EQUAL(code_point, 0x1F600u);
  ASSERT_EQUAL(offset, sizeof(input));
}

void test_decode_invalid_utf8() {
  const char continuation[] = {(char)0x80};
  const char overlong[] = {(char)0xC0, (char)0x80};
  const char invalid_continuation[] = {(char)0xE2, '(', (char)0xA1};
  const char surrogate[] = {(char)0xED, (char)0xA0, (char)0x80};
  const char too_large[] = {(char)0xF4, (char)0x90, (char)0x80, (char)0x80};
  const char truncated[] = {(char)0xF0, (char)0x9F, (char)0x98};
  size_t offset;
  uint32_t code_point;

  offset = 0;
  code_point = 123;
  ASSERT_EQUAL(decode_next_utf8_code_point(continuation,
                                           sizeof(continuation), &offset,
                                           &code_point),
               UNICODE_INVALID_UTF8);
  ASSERT_EQUAL(offset, 0);
  ASSERT_EQUAL(code_point, 123);

  ASSERT_EQUAL(decode_next_utf8_code_point(overlong, sizeof(overlong), &offset,
                                           &code_point),
               UNICODE_INVALID_UTF8);
  ASSERT_EQUAL(decode_next_utf8_code_point(invalid_continuation,
                                           sizeof(invalid_continuation),
                                           &offset, &code_point),
               UNICODE_INVALID_UTF8);
  ASSERT_EQUAL(decode_next_utf8_code_point(surrogate, sizeof(surrogate),
                                           &offset, &code_point),
               UNICODE_INVALID_UTF8);
  ASSERT_EQUAL(decode_next_utf8_code_point(too_large, sizeof(too_large),
                                           &offset, &code_point),
               UNICODE_INVALID_UTF8);
  ASSERT_EQUAL(decode_next_utf8_code_point(truncated, sizeof(truncated),
                                           &offset, &code_point),
               UNICODE_TRUNCATED_UTF8);
  ASSERT_EQUAL(offset, 0);
}

void test_encode_utf8_code_points() {
  char output[4];
  size_t output_length;

  ASSERT_EQUAL(encode_utf8_code_point(0x41u, output, &output_length),
               UNICODE_SUCCESS);
  ASSERT_EQUAL(output_length, 1);
  ASSERT_EQUAL((unsigned char)output[0], 0x41u);

  ASSERT_EQUAL(encode_utf8_code_point(0xA2u, output, &output_length),
               UNICODE_SUCCESS);
  ASSERT_EQUAL(output_length, 2);
  ASSERT_EQUAL((unsigned char)output[0], 0xC2u);
  ASSERT_EQUAL((unsigned char)output[1], 0xA2u);

  ASSERT_EQUAL(encode_utf8_code_point(0x4E2Du, output, &output_length),
               UNICODE_SUCCESS);
  ASSERT_EQUAL(output_length, 3);
  ASSERT_EQUAL((unsigned char)output[0], 0xE4u);
  ASSERT_EQUAL((unsigned char)output[1], 0xB8u);
  ASSERT_EQUAL((unsigned char)output[2], 0xADu);

  ASSERT_EQUAL(encode_utf8_code_point(0x1F600u, output, &output_length),
               UNICODE_SUCCESS);
  ASSERT_EQUAL(output_length, 4);
  ASSERT_EQUAL((unsigned char)output[0], 0xF0u);
  ASSERT_EQUAL((unsigned char)output[1], 0x9Fu);
  ASSERT_EQUAL((unsigned char)output[2], 0x98u);
  ASSERT_EQUAL((unsigned char)output[3], 0x80u);
}

void test_encode_invalid_code_points() {
  char output[4];
  size_t output_length;

  output_length = 99;
  ASSERT_EQUAL(encode_utf8_code_point(0xD800u, output, &output_length),
               UNICODE_INVALID_CODE_POINT);
  ASSERT_EQUAL(output_length, 99);

  ASSERT_EQUAL(encode_utf8_code_point(0x110000u, output, &output_length),
               UNICODE_INVALID_CODE_POINT);
  ASSERT_EQUAL(output_length, 99);
}

void test_utf8_utf32_conversion() {
  const char input[] = {'A',
                        '\0',
                        (char)0xE4,
                        (char)0xB8,
                        (char)0xAD,
                        (char)0xF0,
                        (char)0x9F,
                        (char)0x98,
                        (char)0x80};
  uint32_t *utf32;
  size_t utf32_length;
  char *utf8;
  size_t utf8_length;

  utf32 = NULL;
  utf32_length = 0;
  ASSERT_EQUAL(utf8_to_utf32(input, sizeof(input), &utf32, &utf32_length),
               UNICODE_SUCCESS);
  ASSERT_NOT_EQUAL(utf32, NULL);
  ASSERT_EQUAL(utf32_length, 4);
  ASSERT_EQUAL(utf32[0], 0x41u);
  ASSERT_EQUAL(utf32[1], 0u);
  ASSERT_EQUAL(utf32[2], 0x4E2Du);
  ASSERT_EQUAL(utf32[3], 0x1F600u);

  utf8 = NULL;
  utf8_length = 0;
  ASSERT_EQUAL(utf32_to_utf8(utf32, utf32_length, &utf8, &utf8_length),
               UNICODE_SUCCESS);
  ASSERT_NOT_EQUAL(utf8, NULL);
  ASSERT_EQUAL(utf8_length, sizeof(input));
  ASSERT_EQUAL(memcmp(utf8, input, sizeof(input)), 0);
  ASSERT_EQUAL(utf8[utf8_length], '\0');

  free(utf8);
  free(utf32);
}

void test_unicode_empty_and_invalid_arguments() {
  const char invalid_utf8[] = {(char)0xC0, (char)0x80};
  const uint32_t invalid_utf32[] = {0xD800u};
  uint32_t *utf32;
  size_t utf32_length;
  char *utf8;
  char output[4];
  size_t utf8_length;
  size_t offset;
  uint32_t code_point;

  utf32 = (uint32_t *)1;
  utf32_length = 99;
  ASSERT_EQUAL(utf8_to_utf32(NULL, 0, &utf32, &utf32_length),
               UNICODE_SUCCESS);
  ASSERT_EQUAL(utf32, NULL);
  ASSERT_EQUAL(utf32_length, 0);

  utf8 = (char *)1;
  utf8_length = 99;
  ASSERT_EQUAL(utf32_to_utf8(NULL, 0, &utf8, &utf8_length),
               UNICODE_SUCCESS);
  ASSERT_NOT_EQUAL(utf8, NULL);
  ASSERT_EQUAL(utf8_length, 0);
  ASSERT_EQUAL(utf8[0], '\0');
  free(utf8);

  utf32 = (uint32_t *)1;
  utf32_length = 99;
  ASSERT_EQUAL(utf8_to_utf32(invalid_utf8, sizeof(invalid_utf8), &utf32,
                             &utf32_length),
               UNICODE_INVALID_UTF8);
  ASSERT_EQUAL(utf32, NULL);
  ASSERT_EQUAL(utf32_length, 0);

  utf8 = (char *)1;
  utf8_length = 99;
  ASSERT_EQUAL(utf32_to_utf8(invalid_utf32, 1, &utf8, &utf8_length),
               UNICODE_INVALID_CODE_POINT);
  ASSERT_EQUAL(utf8, NULL);
  ASSERT_EQUAL(utf8_length, 0);

  offset = 0;
  code_point = 0;
  ASSERT_EQUAL(decode_next_utf8_code_point(NULL, 0, &offset, &code_point),
               UNICODE_INVALID_ARGUMENT);
  ASSERT_EQUAL(decode_next_utf8_code_point("", 0, NULL, &code_point),
               UNICODE_INVALID_ARGUMENT);
  ASSERT_EQUAL(decode_next_utf8_code_point("", 0, &offset, NULL),
               UNICODE_INVALID_ARGUMENT);
  ASSERT_EQUAL(encode_utf8_code_point(0, NULL, &utf8_length),
               UNICODE_INVALID_ARGUMENT);
  ASSERT_EQUAL(encode_utf8_code_point(0, output, NULL),
               UNICODE_INVALID_ARGUMENT);
  ASSERT_EQUAL(utf8_to_utf32("", 0, NULL, &utf32_length),
               UNICODE_INVALID_ARGUMENT);
  ASSERT_EQUAL(utf32_to_utf8(NULL, 1, &utf8, &utf8_length),
               UNICODE_INVALID_ARGUMENT);
}
