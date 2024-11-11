// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <lib.h>
#include <stdint.h>

void *memset(void *destination, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t) c;
	char *dst = (char *) destination;

	while (length--)
		dst[length] = chr;

	return destination;
}

void *memcpy(void *destination, const void *source, uint64_t length) {
	/*
	 * memcpy does not support overlapping buffers, so always do it
	 * forwards. (Don't change this without adjusting memmove.)
	 *
	 * For speedy copying, optimize the common case where both pointers
	 * and the length are word-aligned, and copy word-at-a-time instead
	 * of byte-at-a-time. Otherwise, copy by bytes.
	 *
	 * The alignment logic below should be portable. We rely on
	 * the compiler to be reasonably intelligent about optimizing
	 * the divides and modulos out. Fortunately, it is.
	 */
	uint64_t i;

	if ((uint64_t) destination % sizeof(uint32_t) == 0 &&
		(uint64_t) source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0) {
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *) source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else {
		uint8_t *d = (uint8_t *) destination;
		const uint8_t *s = (const uint8_t *) source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

size_t strlen(const char *str) {
	size_t len = 0;
	while (str[len] != 0) {
		len++;
	}
	return len;
}

void reverse(char *str) {
	size_t len = strlen(str);
	for (size_t i = 0; i < len / 2; i++) {
		char temp = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = temp;
	}
}

char *itoa(int num, char *str) {
	int i = 0;
	int isNegative = 0;
	if (num < 0) {
		isNegative = 1;
		num = -num;
	}
	while (num != 0) {
		str[i++] = num % 10 + '0';
		num /= 10;
	}
	if (isNegative) {
		str[i++] = '-';
	}
	str[i] = 0;
	reverse(str);
	return str;
}

char *itoaHex(uint64_t num, char *str) {
	int i = 0;
	while (num != 0) {
		int r = num % 16;
		str[i++] = (r < 10) ? (r + '0') : (r - 10 + 'A');
		num /= 16;
	}
	str[i] = 0;
	reverse(str);
	return str;
}

void strncpy(char *dest, const char *src, size_t size) {
	for (size_t i = 0; i < size; i++) {
		dest[i] = src[i];
		if (src[i] == 0)
			break;
	}
}

int strcat(char *dest, const char *src) {
	size_t dest_len = strlen(dest);
	size_t i;
	for (i = 0; src[i] != '\0'; i++) {
		dest[dest_len + i] = src[i];
	}
	dest[dest_len + i] = '\0';
	return dest_len + i;
}

int strcmp(const char *s1, const char *s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(const unsigned char *) s1 - *(const unsigned char *) s2;
}
