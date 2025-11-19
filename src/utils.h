#ifndef UTILS_H
#define UTILS_H
#include <stddef.h> 

void* safe_malloc(size_t size);

char* safe_strdup(const char* s);

char* read_file_to_buffer(const char* filename);

#endif 
