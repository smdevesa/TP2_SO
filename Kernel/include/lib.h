#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <stddef.h>


void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

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


char * itoaHex(uint64_t num, char * str); 





#endif