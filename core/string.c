#include <string.h>
#include <stdint.h>

char *strcat(char *dest, const char *src)
{
	int i, k;

	for (i = strlen(dest), k = 0; k < strlen(src); i++, k++) {
		dest[i] = src[k];
	}

	dest[i] = '\0';
	return dest;
}

char *strrev(char *str)
{
	char tmp;
	int i, j, n;
	n = strlen(str);

	for (i = 0, j = n - 1; i < j; ++i, --j) {
		tmp = str[j];
		str[j] = str[i];
		str[i] = tmp;
	}
	
	return str;
}

char *strcpy(char *dest, const char *src)
{
	char *save = dest;
	for (; (*dest = *src) != '\0'; ++src, ++dest);

	return save;
}

char *strncpy(char *dest, const char *src, size_t len)
{
	char *save = dest;
	char *s = src;
	for (; (*dest = *src) != '\0' && (src - s) < len; ++src, ++dest);

	return save;
}

int strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2++) {
		if (*s1++ == '\0') {
			return 0;
		}
	}

	return (*(unsigned char *)s1 - *(unsigned char *)--s2);
}

int strncmp(const char *s1, const char *s2, size_t len)
{
	do {
		if (*s1 != *s2++)
			return (*(unsigned char *)s1 - *(unsigned char *)--s2);
		if (*s1++ == '\0')
			break;
	} while (--len != 0);

	return 0;
}

size_t strlen(const char *str)
{
	const char *s = str;
	while (*s)
		s++;
	
	return (s - str);
}

void *memcpy(void *dest, const void *src, size_t n)
{
	uint8_t *pdest = (uint8_t *)dest;
	const uint8_t *psrc = (const uint8_t *)src;

	for (size_t i = 0; i < n; i++) {
		pdest[i] = psrc[i];
	}

	return dest;
}

void *memset(void *s, int c, size_t n)
{
	uint8_t *p = (uint8_t *)s;
 
	for (size_t i = 0; i < n; i++) {
		p[i] = (uint8_t)c;
	}
 
	return s;
}
 
void *memmove(void *dest, const void *src, size_t n)
{
	uint8_t *pdest = (uint8_t *)dest;
	const uint8_t *psrc = (const uint8_t *)src;
 
	if (src > dest) {
		for (size_t i = 0; i < n; i++) {
			pdest[i] = psrc[i];
		}
	} else if (src < dest) {
		for (size_t i = n; i > 0; i--) {
			pdest[i-1] = psrc[i-1];
		}
	}
 
	return dest;
}
 
int memcmp(const void *s1, const void *s2, size_t n)
{
	const uint8_t *p1 = (const uint8_t *)s1;
	const uint8_t *p2 = (const uint8_t *)s2;
 
	for (size_t i = 0; i < n; i++) {
		if (p1[i] != p2[i]) {
			return p1[i] < p2[i] ? -1 : 1;
		}
	}
 
	return 0;
}