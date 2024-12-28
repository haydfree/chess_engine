#include <stdio.h>
#include <stdlib.h>

void* debug_malloc(size_t size, const char* file, int line) {
    #undef malloc
    void* ptr = malloc(size);
    #define malloc(size) debug_malloc(size, __FILE__, __LINE__)
    printf("[MALLOC] Allocated %zu bytes at %p (%s:%d)\n", size, ptr, file, line);
    return ptr;
}
void debug_free(void* ptr, const char* file, int line) {
    if (ptr == NULL) {
        printf("[FREE] Attempt to free NULL pointer (%s:%d)\n", file, line);
        return;
    }
    printf("[FREE] Deallocated memory at %p (%s:%d)\n", ptr, file, line);
    #undef free
    free(ptr);
    #define free(ptr) debug_free(ptr, __FILE__, __LINE__)
}
