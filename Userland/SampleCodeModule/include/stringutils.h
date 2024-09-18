#ifndef TPE_ARQ_STRINGUTILS_H
#define TPE_ARQ_STRINGUTILS_H

#include <stddef.h>

/**
 * @brief Returns the length of a null-terminated string.
 * @param str
 */
size_t strlen(const char *str);

/**
 * @brief Copies a null-terminated string.
 * @param dest The destination buffer.
 * @param src The source buffer.
 */
void strcpy(char *dest, const char *src);

/**
 * @brief Compares two null-terminated strings.
 * @param str1 The first string.
 * @param str2 The second string.
 * @return 0 if the strings are equal, a negative number if str1 is less than str2, a positive number if str1 is greater than str2.
 */
int strcmp(const char *str1, const char *str2);

/**
 * @brief Copies a null-terminated string up to n characters.
 * @param dest The destination buffer.
 * @param src The source buffer.
 * @param n The number of characters to copy.
 */
void strncpy(char *dest, const char *src, size_t n);

/**
 * @brief Reverses a null-terminated string.
 * @param str The string to reverse.
 */
void reverse(char *str);

#endif //TPE_ARQ_STRINGUTILS_H
