#include "logger.h"

Logger logger;

#ifdef FLINT_VM_DEBUG_MODE

int log_init(const char *file_name, LogLevel level) {
  logger.file = fopen(file_name, "w");
  if (logger.file) {
    logger.level = level;

    return 0;
  } else {
    printf("Failed to open log file: %s\n", file_name);

    return -1;
  }
}

void log_close() {
  if (logger.file) {
    fclose(logger.file);
    logger.file = NULL;
  }
}

void log_message(LogLevel level, const char *format, ...) {
  if (level < logger.level) {
    return;
  }

  time_t t = time(NULL);
  struct tm *tm_info = localtime(&t);
  char time_buf[20];
  strftime(time_buf, 20, "%Y-%m-%d %H:%M:%S", tm_info);

  const char *level_str = "";
  switch (level) {
  case LOG_LEVEL_DEBUG:
    level_str = "DEBUG";
    break;
  case LOG_LEVEL_INFO:
    level_str = "INFO";
    break;
  case LOG_LEVEL_WARN:
    level_str = "WARN";
    break;
  case LOG_LEVEL_ERROR:
    level_str = "ERROR";
    break;
  }

  va_list args;
  if (logger.file) {
    va_start(args, format);
    fprintf(logger.file, "[%s] %s: ", time_buf, level_str);
    vfprintf(logger.file, format, args);
    fprintf(logger.file, "\n");
    va_end(args);
  } else {
    /* If no log file is open, log messages will be printed to the console. */
    va_start(args, format);
    printf("[%s] %s: ", time_buf, level_str);
    vprintf(format, args);
    printf("\n");
    va_end(args);
  }
}

#endif /* FLINT_VM_DEBUG_MODE */