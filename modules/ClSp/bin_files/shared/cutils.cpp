#include <cutils.h>
#include <Windows.h>

int _strcmp(const char* str1, const char* str2) {
    while (*str1 != '\0' && *str1 == *str2) {
        str1++;
        str2++;
    }
    return (*(unsigned char*)str1) - (*(unsigned char*)str2);
}

int _wcscmp(const wchar_t* wstr1, const wchar_t* wstr2) {
    while (*wstr1 != L'\0' && *wstr1 == *wstr2) {
        wstr1++;
        wstr2++;
    }
    return (*wstr1) - (*wstr2);
}

void __memset(void* dest, int val, size_t size) {
    for (size_t i = 0; i < size; i++)
    {
        *(PCHAR)((PCHAR)dest + i) = (char)val;
    }
}
void _memset(void* dest, int val, size_t size) {
    memset(dest, val, size);
}
void _memcpy(void* dest, void* src, size_t size) {
    memcpy(dest, src, size);
}
void __memcpy(void* dest, void* src, size_t size) {
    for (size_t i = 0; i < size; i++)
    {
        ((PCHAR)dest)[i] = ((PCHAR)src)[i];
    }
}

int _strlen(char* s) {
    int i = 0;
    while (*s != 0) {
        i++;
        s++;
    }
    return i;
}

int _wcslen(wchar_t* ws) {
    int i = 0;
    while (*ws != 0 && *(ws + 1) != 0) {
        i++;
        ws++;
    }
    return i + 1;
}

void _strlow(char* ws, int len, char* outws) {
    for (int i = 0; i < len; i++) {
        *(outws + i) = *(ws + i);
        if (*(ws + i) >= 65 && *(ws + i) <= 90) *(outws + i) = *(ws + i) + 32;
    }
}

bool is_data_null(char *data, int len) {
    for (int i = 0; i < len; i++)
    {
        if (data[i] != NULL) return false;
    }
    return true;
}