#ifndef LIB_H
#define LIB_H

#include <stddef.h>
#include <stdint.h>

void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);

char *cpuVendor(char *result);

/**
 * @brief Returns the length of a null-terminated string.
 * @param str
 */
size_t strlen(const char *str);

/**
 * @brief Reverses a null-terminated string.
 * @param str The string to reverse.
 */
void reverse(char *str);

char *itoaHex(uint64_t num, char *str);

void strncpy(char *dest, const char *src, size_t size);

char *itoa(int num, char *str);

int strcat(char *dest, const char *src);

int strcmp(const char *s1, const char *s2);

#endif