#pragma once
int _wcscmp(const wchar_t* wstr1, const wchar_t* wstr2);
int _strcmp(const char* str1, const char* str2);
void _memset(void* dest, int val, size_t size);
void _memcpy(void* dest, void* src, size_t size);
int _strlen(char* s);
int _wcslen(wchar_t* ws);
void _strlow(char* ws, int len, char* outws);
bool is_data_null(char* data, int len);