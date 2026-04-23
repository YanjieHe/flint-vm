#ifndef FLINT_VM_LOGGER_H
#define FLINT_VM_LOGGER_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef enum {
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR
} LogLevel;

typedef struct {
  FILE *file;
  LogLevel level;
} Logger;

extern Logger logger;

#ifdef FLINT_VM_DEBUG_MODE
int log_init(const char *file_name, LogLevel level);
void set_log_level(LogLevel level);
void log_close();
void log_message_impl(LogLevel level, const char *format, ...);
#define log_message(x) log_message_impl x

#else

#define log_init(file_name, level) ((void)0)
#define log_close() ((void)0)
#define log_message(x) ((void)0)

#endif /* FLINT_VM_DEBUG_MODE */

#endif /* FLINT_VM_LOGGER_H */