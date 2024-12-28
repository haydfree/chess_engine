#ifndef MEMORY_DEBUG_H
#define MEMORY_DEBUG_H

#define malloc(size) debug_malloc(size, __FILE__, __LINE__)
#define free(ptr) debug_free(ptr, __FILE__, __LINE__)
extern void* debug_malloc(int size, const char* file, int line);
extern void debug_free(void* ptr, const char* file, int line);

#endif //MEMORY_DEBUG_H
