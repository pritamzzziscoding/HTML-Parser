/**
 * src/utils.h
 *
 * Header for utility functions, like memory-safe wrappers.
 */
#ifndef UTILS_H
#define UTILS_H

#include <stddef.h> // For size_t

/**
 * @brief A wrapper around malloc that checks for allocation failure
 * and exits the program if memory cannot be allocated.
 */
void* safe_malloc(size_t size);

/**
 * @brief A wrapper around strdup that uses safe_malloc.
 */
char* safe_strdup(const char* s);

/**
 * @brief Reads an entire file into a heap-allocated string.
 * The caller is responsible for freeing the returned string.
 * Returns NULL on failure.
 */
char* read_file_to_buffer(const char* filename);

#endif // UTILS_H
