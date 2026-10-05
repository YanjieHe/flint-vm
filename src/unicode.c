#include "unicode.h"

#include <stdlib.h>
#include <string.h>

static int is_utf8_continuation_byte(unsigned char byte) {
  return (byte & 0xC0u) == 0x80u;
}

static int is_unicode_scalar_value(uint32_t code_point) {
  return code_point <= 0x10FFFFu &&
         !(code_point >= 0xD800u && code_point <= 0xDFFFu);
}

UnicodeStatus decode_next_utf8_code_point(const char *utf8, size_t byte_length,
                                          size_t *offset,
                                          uint32_t *code_point) {
  const unsigned char *bytes;
  size_t current;
  size_t sequence_length;
  uint32_t result;
  unsigned char first;
  unsigned char second;

  if (utf8 == NULL || offset == NULL || code_point == NULL ||
      *offset > byte_length) {
    return UNICODE_INVALID_ARGUMENT;
  }

  current = *offset;
  if (current == byte_length) {
    return UNICODE_TRUNCATED_UTF8;
  }

  bytes = (const unsigned char *)utf8;
  first = bytes[current];

  if (first <= 0x7Fu) {
    sequence_length = 1;
    result = first;
  } else if (first >= 0xC2u && first <= 0xDFu) {
    sequence_length = 2;
    result = first & 0x1Fu;
  } else if (first >= 0xE0u && first <= 0xEFu) {
    sequence_length = 3;
    result = first & 0x0Fu;
  } else if (first >= 0xF0u && first <= 0xF4u) {
    sequence_length = 4;
    result = first & 0x07u;
  } else {
    return UNICODE_INVALID_UTF8;
  }

  if (sequence_length > byte_length - current) {
    return UNICODE_TRUNCATED_UTF8;
  }

  if (sequence_length >= 2) {
    second = bytes[current + 1];
    if (!is_utf8_continuation_byte(second)) {
      return UNICODE_INVALID_UTF8;
    }

    if ((first == 0xE0u && second < 0xA0u) ||
        (first == 0xEDu && second > 0x9Fu) ||
        (first == 0xF0u && second < 0x90u) ||
        (first == 0xF4u && second > 0x8Fu)) {
      return UNICODE_INVALID_UTF8;
    }

    result = (result << 6) | (second & 0x3Fu);
  }

  if (sequence_length >= 3) {
    if (!is_utf8_continuation_byte(bytes[current + 2])) {
      return UNICODE_INVALID_UTF8;
    }
    result = (result << 6) | (bytes[current + 2] & 0x3Fu);
  }

  if (sequence_length == 4) {
    if (!is_utf8_continuation_byte(bytes[current + 3])) {
      return UNICODE_INVALID_UTF8;
    }
    result = (result << 6) | (bytes[current + 3] & 0x3Fu);
  }

  if (!is_unicode_scalar_value(result)) {
    return UNICODE_INVALID_UTF8;
  }

  *code_point = result;
  *offset = current + sequence_length;
  return UNICODE_SUCCESS;
}

UnicodeStatus encode_utf8_code_point(uint32_t code_point, char output[4],
                                     size_t *output_length) {
  if (output == NULL || output_length == NULL) {
    return UNICODE_INVALID_ARGUMENT;
  }

  if (!is_unicode_scalar_value(code_point)) {
    return UNICODE_INVALID_CODE_POINT;
  }

  if (code_point <= 0x7Fu) {
    output[0] = (char)code_point;
    *output_length = 1;
  } else if (code_point <= 0x7FFu) {
    output[0] = (char)(0xC0u | (code_point >> 6));
    output[1] = (char)(0x80u | (code_point & 0x3Fu));
    *output_length = 2;
  } else if (code_point <= 0xFFFFu) {
    output[0] = (char)(0xE0u | (code_point >> 12));
    output[1] = (char)(0x80u | ((code_point >> 6) & 0x3Fu));
    output[2] = (char)(0x80u | (code_point & 0x3Fu));
    *output_length = 3;
  } else {
    output[0] = (char)(0xF0u | (code_point >> 18));
    output[1] = (char)(0x80u | ((code_point >> 12) & 0x3Fu));
    output[2] = (char)(0x80u | ((code_point >> 6) & 0x3Fu));
    output[3] = (char)(0x80u | (code_point & 0x3Fu));
    *output_length = 4;
  }

  return UNICODE_SUCCESS;
}

UnicodeStatus utf8_to_utf32(const char *utf8, size_t utf8_length,
                            uint32_t **utf32, size_t *utf32_length) {
  UnicodeStatus status;
  uint32_t code_point;
  uint32_t *result;
  size_t offset;
  size_t character_count;
  size_t i;

  if (utf32 == NULL || utf32_length == NULL) {
    return UNICODE_INVALID_ARGUMENT;
  }

  *utf32 = NULL;
  *utf32_length = 0;

  if (utf8 == NULL && utf8_length != 0) {
    return UNICODE_INVALID_ARGUMENT;
  }

  offset = 0;
  character_count = 0;
  while (offset < utf8_length) {
    status =
        decode_next_utf8_code_point(utf8, utf8_length, &offset, &code_point);
    if (status != UNICODE_SUCCESS) {
      return status;
    }
    character_count++;
  }

  if (character_count == 0) {
    return UNICODE_SUCCESS;
  }

  if (character_count > (size_t)-1 / sizeof(uint32_t)) {
    return UNICODE_LENGTH_OVERFLOW;
  }

  result = malloc(sizeof(uint32_t) * character_count);
  if (result == NULL) {
    return UNICODE_OUT_OF_MEMORY;
  }

  offset = 0;
  i = 0;
  while (offset < utf8_length) {
    status =
        decode_next_utf8_code_point(utf8, utf8_length, &offset, &result[i]);
    if (status != UNICODE_SUCCESS) {
      free(result);
      return status;
    }
    i++;
  }

  *utf32 = result;
  *utf32_length = character_count;
  return UNICODE_SUCCESS;
}

UnicodeStatus utf32_to_utf8(const uint32_t *utf32, size_t utf32_length,
                            char **utf8, size_t *utf8_length) {
  UnicodeStatus status;
  char encoded[4];
  char *result;
  size_t encoded_length;
  size_t byte_count;
  size_t offset;
  size_t i;

  if (utf8 == NULL || utf8_length == NULL) {
    return UNICODE_INVALID_ARGUMENT;
  }

  *utf8 = NULL;
  *utf8_length = 0;

  if (utf32 == NULL && utf32_length != 0) {
    return UNICODE_INVALID_ARGUMENT;
  }

  byte_count = 0;
  for (i = 0; i < utf32_length; i++) {
    status = encode_utf8_code_point(utf32[i], encoded, &encoded_length);
    if (status != UNICODE_SUCCESS) {
      return status;
    }

    if (byte_count > (size_t)-1 - encoded_length - 1) {
      return UNICODE_LENGTH_OVERFLOW;
    }
    byte_count += encoded_length;
  }

  result = malloc(byte_count + 1);
  if (result == NULL) {
    return UNICODE_OUT_OF_MEMORY;
  }

  offset = 0;
  for (i = 0; i < utf32_length; i++) {
    status = encode_utf8_code_point(utf32[i], encoded, &encoded_length);
    if (status != UNICODE_SUCCESS) {
      free(result);
      return status;
    }
    memcpy(result + offset, encoded, encoded_length);
    offset += encoded_length;
  }
  result[byte_count] = '\0';

  *utf8 = result;
  *utf8_length = byte_count;
  return UNICODE_SUCCESS;
}
