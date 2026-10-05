#ifndef FLINT_VM_UNICODE_H
#define FLINT_VM_UNICODE_H

#include <stddef.h>
#include <stdint.h>

typedef enum UnicodeStatus {
  UNICODE_SUCCESS = 0,
  UNICODE_INVALID_ARGUMENT,
  UNICODE_INVALID_UTF8,
  UNICODE_TRUNCATED_UTF8,
  UNICODE_INVALID_CODE_POINT,
  UNICODE_OUT_OF_MEMORY,
  UNICODE_LENGTH_OVERFLOW
} UnicodeStatus;

UnicodeStatus decode_next_utf8_code_point(const char *utf8, size_t byte_length,
                                          size_t *offset, uint32_t *code_point);

UnicodeStatus encode_utf8_code_point(uint32_t code_point, char output[4],
                                     size_t *output_length);

/* On success, the caller owns *utf32 and must free it. */
UnicodeStatus utf8_to_utf32(const char *utf8, size_t utf8_length,
                            uint32_t **utf32, size_t *utf32_length);

/*
 * On success, the caller owns *utf8 and must free it. The result is NUL
 * terminated, but the terminator is not included in *utf8_length.
 */
UnicodeStatus utf32_to_utf8(const uint32_t *utf32, size_t utf32_length,
                            char **utf8, size_t *utf8_length);

#endif /* FLINT_VM_UNICODE_H */
