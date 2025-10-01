#ifndef __APPLE__
#error "This library currently only supports macOS."
#endif

#ifdef __cplusplus__
#error "This library does not support C++ as of now."
#endif

#ifndef LKSAN_H
#define LKSAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <execinfo.h>

#ifndef LKSAN_NO_MACRO

#define malloc(sz) lks_malloc(__func__, __LINE__, __FILE__, sz, false)
#define free(sz) lks_free(__func__, __LINE__, __FILE__, sz)
#define realloc(ptr, sz) lks_realloc(__func__, __LINE__, __FILE__, ptr, sz)
#define calloc(sz) lks_calloc(__func__, __LINE__, __FILE__, sz)
#define strdup(ptr) lks_strdup(__func__, __LINE__, __FILE__, ptr)
#define strndup(ptr, sz) lks_strndup(__func__, __LINE__, __FILE__, ptr, sz)

#endif // LKSAN_NO_MACRO

typedef struct AllocInfo{
    size_t size;
    const char* file;
    int line;
    const void* ptr;
    char* stack_trace;
    struct AllocInfo* next;
} AllocInfo;

void* lks_malloc(const char* func, int line, const char* file, size_t size, bool skip_2nd_stack);
void lks_free(const char* func, int line, const char* file, void* ptr);
void* lks_realloc(const char* func, int line, const char* file, void* ptr, size_t size);
void* lks_calloc(const char* func, int line, const char* file, size_t size);
char* lks_strdup(const char* func, int line, const char* file, const char* ptr);
char* lks_strndup(const char* func, int line, const char* file, const char* ptr, size_t size);
void lksan_report_leaks(FILE* out, bool free_list);
void __attribute__((destructor)) lksan_report_leaks_atexit(void);

#endif // LKSAN_H

