#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>

class CommandParser
{
public:
	BYTE get_byte();
	BYTE get_command();
	DWORD get_int();
	SHORT get_short();
	LONGLONG get_long();
	std::string get_strarg();
	std::string get_str(size_t size);
	void load_data(std::vector<UCHAR> data);
	void load_data(char* data, size_t size);

	~CommandParser();
private:
	std::vector<char> input;

};