#define LKSAN_NO_MACRO
#include "lksan.h"

static AllocInfo* alloc_list = NULL;
static size_t total_allocated = 0;
static bool has_reported = false;

char* get_stack_trace(int stack_skip) {
    void *buffer[100];
    int nptrs = backtrace(buffer, 100);        // capture up to 100 frames
    char **symbols = backtrace_symbols(buffer, nptrs);

    if (symbols == NULL) {
        return strdup("[Failed to get stack trace]\n");
    }

    // Calculate total length
    size_t total_len = 0;
    for (int i = 1; i < nptrs; i++) {         // skip first frame
        total_len += strlen(symbols[i]) + 1; // +1 for newline
    }

    char *trace_str = malloc(total_len + 1);  // +1 for terminating null
    if (!trace_str) {
        free(symbols);
        return strdup("Out of memory\n");
    }

    trace_str[0] = '\0';
    for (int i = stack_skip+1; i < nptrs-1; i++) {
        strcat(trace_str, symbols[i]);
        strcat(trace_str, "\n");
    }

    free(symbols);
    return trace_str;
}

void lks_free(const char* func, int line, const char* file, void* ptr) {
    if (!ptr) return;
    AllocInfo** current = &alloc_list;
    while (*current) {
        if ((*current)->ptr == ptr) {
            AllocInfo* to_free = *current;
            *current = (*current)->next;
            total_allocated -= to_free->size;
            free(to_free->stack_trace);
            free(to_free);
            break;
        }
        current = &(*current)->next;
    }
    free(ptr);
}

void* lks_malloc(const char* func, int line, const char* file, size_t size, bool skip_2nd_stack) {
    void* ptr = malloc(size);
    if (ptr) {
        AllocInfo* info = (AllocInfo*)malloc(sizeof(AllocInfo));
        info->size = size;
        info->file = file;
        info->line = line;
        info->ptr = ptr;
        info->stack_trace = get_stack_trace(skip_2nd_stack ? 2 : 1);
        info->next = alloc_list;
        alloc_list = info;
        total_allocated += size;
    }
    return ptr;
}

void* lks_realloc(const char* func, int line, const char* file, void* ptr, size_t size) {
    if (!ptr) return lks_malloc(func, line, file, size, false);
    AllocInfo** current = &alloc_list;
    while (*current) {
        if ((*current)->ptr == ptr) {
            AllocInfo* info = *current;
            void* new_ptr = realloc(ptr, size);
            if (new_ptr) {
                total_allocated += (size - info->size);
                info->size = size;
                info->ptr = new_ptr;
                free(info->stack_trace);
                info->stack_trace = get_stack_trace(1);
            }
            return new_ptr;
        }
        current = &(*current)->next;
    }
    return NULL; // Pointer not found
}

void* lks_calloc(const char* func, int line, const char* file, size_t size) {
    void* ptr = calloc(1, size);
    if (ptr) {
        AllocInfo* info = (AllocInfo*)malloc(sizeof(AllocInfo));
        info->size = size;
        info->file = file;
        info->line = line;
        info->ptr = ptr;
        info->stack_trace = get_stack_trace(1);
        info->next = alloc_list;
        alloc_list = info;
        total_allocated += size;
    }
    return ptr;
}

char* lks_strdup(const char* func, int line, const char* file, const char* ptr) {
    if (!ptr) return NULL;
    size_t size = strlen(ptr) + 1;
    char* new_ptr = (char*)lks_malloc(func, line, file, size, true);
    if (new_ptr) {
        memcpy(new_ptr, ptr, size);
    }
    return new_ptr;
}

char* lks_strndup(const char* func, int line, const char* file, const char* ptr, size_t size) {
    if (!ptr) return NULL;
    size_t len = strnlen(ptr, size);
    char* new_ptr = (char*)lks_malloc(func, line, file, len + 1, true);
    if (new_ptr) {
        memcpy(new_ptr, ptr, len);
        new_ptr[len] = '\0';
    }
    return new_ptr;
}

void lksan_report_leaks(FILE* out, bool free_list) {
    AllocInfo* current = alloc_list;
    if (!current) {
        fprintf(out, "No memory leaks detected.\n");
        return;
    }
    fprintf(out, "Memory leaks detected:\n");
    while (current) {
        fprintf(out, "Leaked %zu bytes allocated at %s:%d at call stack: \n%s\n\n", current->size, current->file, current->line, current->stack_trace);
        current = current->next;
    }
    fprintf(out, "Total leaked memory: %zu bytes\n", total_allocated);
    if (!free_list) return;

    has_reported = true;
    
    // Free the allocation list
    current = alloc_list;
    while (current) {
        AllocInfo* to_free = current;
        current = current->next;
        free(to_free->stack_trace);
        free(to_free);
    }

    alloc_list = NULL;
}

void __attribute__((destructor)) lksan_report_leaks_atexit(void) {
    if (!has_reported) {
        lksan_report_leaks(stderr, true);
    }
}



