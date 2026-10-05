#include "test_byte_code_loader.h"

#include "byte_code_loader.h"
#include "test.h"

static FILE *create_byte_code_stream(const Byte *bytes, size_t length) {
  FILE *file;

  file = tmpfile();
  if (file == NULL) {
    return NULL;
  }

  if (fwrite(bytes, sizeof(Byte), length, file) != length) {
    fclose(file);
    return NULL;
  }

  rewind(file);
  return file;
}

static void initialize_loader(ByteCodeLoader *loader, FILE *file) {
  loader->file_name = NULL;
  loader->file = file;
  loader->error_messages = NULL;
  loader->native_library_resolver = NULL;
}

static void close_loader_stream(ByteCodeLoader *loader) {
  ErrorList *error;
  ErrorList *next;

  error = loader->error_messages;
  while (error != NULL) {
    next = error->next;
    free(error->message);
    free(error);
    error = next;
  }

  fclose(loader->file);
}

void test_read_utf8_string() {
  const Byte bytes[] = {0,
                        8,
                        'A',
                        0xE4,
                        0xB8,
                        0xAD,
                        0xF0,
                        0x9F,
                        0x98,
                        0x80};
  ByteCodeLoader loader;
  String *string;
  FILE *file;

  file = create_byte_code_stream(bytes, sizeof(bytes));
  ASSERT_NOT_EQUAL(file, NULL);
  if (file == NULL) {
    return;
  }

  initialize_loader(&loader, file);
  string = read_string(&loader);

  ASSERT_EQUAL(loader.error_messages, NULL);
  ASSERT_NOT_EQUAL(string, NULL);
  if (string != NULL) {
    ASSERT_EQUAL(string->length, 3);
    ASSERT_EQUAL(string->characters[0], 0x41u);
    ASSERT_EQUAL(string->characters[1], 0x4E2Du);
    ASSERT_EQUAL(string->characters[2], 0x1F600u);
    free_string(string);
  }

  close_loader_stream(&loader);
}

void test_read_empty_string() {
  const Byte bytes[] = {0, 0};
  ByteCodeLoader loader;
  String *string;
  FILE *file;

  file = create_byte_code_stream(bytes, sizeof(bytes));
  ASSERT_NOT_EQUAL(file, NULL);
  if (file == NULL) {
    return;
  }

  initialize_loader(&loader, file);
  string = read_string(&loader);

  ASSERT_EQUAL(loader.error_messages, NULL);
  ASSERT_NOT_EQUAL(string, NULL);
  if (string != NULL) {
    ASSERT_EQUAL(string->length, 0);
    ASSERT_EQUAL(string->characters, NULL);
    free_string(string);
  }

  close_loader_stream(&loader);
}

void test_read_invalid_utf8_string() {
  const Byte bytes[] = {0, 2, 0xC0, 0x80};
  ByteCodeLoader loader;
  String *string;
  FILE *file;

  file = create_byte_code_stream(bytes, sizeof(bytes));
  ASSERT_NOT_EQUAL(file, NULL);
  if (file == NULL) {
    return;
  }

  initialize_loader(&loader, file);
  string = read_string(&loader);

  ASSERT_EQUAL(string, NULL);
  ASSERT_NOT_EQUAL(loader.error_messages, NULL);

  close_loader_stream(&loader);
}

void test_read_truncated_string() {
  const Byte bytes[] = {0, 3, 'a', 'b'};
  ByteCodeLoader loader;
  String *string;
  FILE *file;

  file = create_byte_code_stream(bytes, sizeof(bytes));
  ASSERT_NOT_EQUAL(file, NULL);
  if (file == NULL) {
    return;
  }

  initialize_loader(&loader, file);
  string = read_string(&loader);

  ASSERT_EQUAL(string, NULL);
  ASSERT_NOT_EQUAL(loader.error_messages, NULL);

  close_loader_stream(&loader);
}
