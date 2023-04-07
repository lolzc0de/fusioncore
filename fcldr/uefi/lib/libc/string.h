#ifndef FCLDR_UEFI_BOOT_STRING_H
#define FCLDR_UEFI_BOOT_STRING_H

#include <stddef.h>

#define NULL 0

void* memcpy(void* destination, const void* source, size_t num);
// void* memmove(void* dest, const void* src, size_t num);
int memcmp(const void* ptr1, const void* ptr2, size_t num);
void* memchr(const void * ptr, int value, size_t num);
void* memset(void* ptr, int value, size_t num);

size_t wcslen(const wchar_t* str);
size_t wcscmp(const wchar_t* str1, const wchar_t* str2);

#endif // FCLDR_UEFI_BOOT_STRING_H
