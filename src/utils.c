#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Fatal: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

char* safe_strdup(const char* s) {
    if (s == NULL) {
        return NULL;
    }
    size_t len = strlen(s) + 1;
    char* new_str = (char*)safe_malloc(len);
    memcpy(new_str, s, len);
    return new_str;
}

char* read_file_to_buffer(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (file_size == -1) {
        perror("Error getting file size");
        fclose(file);
        return NULL;
    }
    fseek(file, 0, SEEK_SET);
    char* buffer = (char*)safe_malloc(file_size + 1);
    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read < (size_t)file_size) {
        if (ferror(file)) {
            perror("Error reading file");
        } else {
            fprintf(stderr, "Error: Incomplete file read.\n");
        }
        fclose(file);
        free(buffer);
        return NULL;
    }
    buffer[file_size] = '\0';
    fclose(file);
    return buffer;
}
