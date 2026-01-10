#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>


class ResponseBuilder
{
public:
	void add_byte(BYTE n);
	void add_int(int n);
	void add_short(short n);
	void add_long(long long n);
	void add_strarg(std::string s);
	void add_bstrarg(char* s, long long len);
	void add_str(std::string s);
	void add_str(char* s, long long len);
	void padding(long long len);
	std::vector<UCHAR> build();
private:

	std::vector<unsigned char> input;

};