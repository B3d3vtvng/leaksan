#include <lksan.h>

#include <stdio.h>
#include <stdlib.h>

// Example custom allocator that uses lksan for tracking
void* custom_alloc_impl(const char* func, int line, const char* file, size_t size) {
    printf("Custom allocating %zu bytes\n", size);
    //Simply use lksan's internal api for tracking
    void* ptr = lks_malloc(func, line, file, size, true); // Skip 2nd stack frame to avoid showing this function in the trace
    return ptr; // This will be tracked by lksan
}

void custom_free_impl(const char* func, int line, const char* file, void* ptr) {
    printf("Custom freeing memory: %p\n", ptr);
    lks_free(func, line, file, ptr); // This will be tracked by lksan
}

//Get the function name, line number and file name automatically by using this macro structure
#define custom_alloc(sz) custom_alloc_impl(__func__, __LINE__, __FILE__, sz)
#define custom_free(ptr) custom_free_impl(__func__, __LINE__, __FILE__, ptr)

int main(void){
    int* leak = custom_alloc(10 * sizeof(int)); // Intentional leak
    lksan_report_leaks(stdout, true); // Report leaks (will be found here, even though we free later)
    custom_free(leak); // Freeing the allocated memory
    return 0;
}