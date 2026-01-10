#include "inc/cmd_parser.hpp"
#include <iostream>
#include <cutils.h>

void CommandParser::load_data(std::vector<UCHAR> data) {
	//input = data;
	input.resize(data.size());
	input.assign(data.begin(), data.end());
}
void CommandParser::load_data(char* data, size_t size) {
	std::vector<unsigned char> temp_vec(data, data + size);
	load_data(temp_vec);
}

BYTE CommandParser::get_byte() {
	BYTE byte = *input.data();
	input.erase(input.begin());
	return byte;
}
BYTE CommandParser::get_command() {
	return get_byte();
}
DWORD CommandParser::get_int() {
	DWORD n = DWORD(
		(BYTE)(input[0]) << 24 |
		(BYTE)(input[1]) << 16 |
		(BYTE)(input[2]) << 8 |
		(BYTE)(input[3])
	);
	input.erase(input.begin(), input.begin() + 4);
	return n;
}

SHORT CommandParser::get_short() {
	SHORT n = SHORT(
		(BYTE)(input[0]) << 8 |
		(BYTE)(input[1])
	);
	input.erase(input.begin(), input.begin() + 2);
	return n;
}

LONGLONG CommandParser::get_long() {
	LONGLONG n;
	_memcpy(&n, input.data(), 8);
	n = _byteswap_uint64(n);
	input.erase(input.begin(), input.begin() + 8);
	return n;
}
std::string CommandParser::get_strarg() {
	DWORD str_size = get_int();
	return get_str(str_size);
}
std::string CommandParser::get_str(size_t size) {
	std::string str(input.begin(), input.begin() + size);
	input.erase(input.begin(), input.begin() + size);
	return str;
}

CommandParser::~CommandParser() {
	input.~vector();
}