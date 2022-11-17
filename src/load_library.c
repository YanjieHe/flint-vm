#include "load_library.h"

#ifdef _WIN32
#define RUNNING_ON_WINDOWS
#endif

#ifdef RUNNING_ON_WINDOWS
#include <windows.h>
#else
#include <dlfcn.h>
#endif

void *open_dynamic_library(const char *library_path) {
#ifdef RUNNING_ON_WINDOWS
  return LoadLibrary(library_path);
#else
  return dlopen(library_path, RTLD_NOW);
#endif
}

void *load_function_from_dynamic_library(void *library_pointer,
                                         const char *function_name) {
#ifdef RUNNING_ON_WINDOWS
  return GetProcAddress((HMODULE)library_pointer, function_name);
#else
  dlsym(library_pointer, function_name);
#endif
}

void close_dynamic_library(void *library_pointer) {
#ifdef RUNNING_ON_WINDOWS
  FreeLibrary((HMODULE)library_pointer);
#else
  dlclose(library_pointer);
#endif
}