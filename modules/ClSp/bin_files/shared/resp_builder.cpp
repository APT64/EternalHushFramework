#include "inc/resp_builder.hpp"
void ResponseBuilder::add_byte(BYTE n) {
	input.push_back(n);
}

void ResponseBuilder::add_short(short n) {
	input.push_back((n & 0xff00) >> 8);
	input.push_back(n & 0xff);
}

void ResponseBuilder::add_int(int n) {
	input.push_back((n & 0xff000000) >> 24);
	input.push_back((n & 0xff0000) >> 16);
	input.push_back((n & 0xff00) >> 8);
	input.push_back(n & 0xff);
}

void ResponseBuilder::add_long(long long n) {
	input.push_back((n & 0xff00000000000000) >> 56);
	input.push_back((n & 0xff000000000000) >> 48);
	input.push_back((n & 0xff0000000000) >> 40);
	input.push_back((n & 0xff00000000) >> 32);
	input.push_back((n & 0xff000000) >> 24);
	input.push_back((n & 0xff0000) >> 16);
	input.push_back((n & 0xff00) >> 8);
	input.push_back(n & 0xff);
}

void ResponseBuilder::add_str(std::string s) {
	for (int i = 0; i < s.length(); i++)
	{
		input.push_back(s.data()[i]);
	}
}

void ResponseBuilder::add_str(char* s, long long len) {
	for (int i = 0; i < len; i++)
	{
		input.push_back(s[i]);
	}
}

void ResponseBuilder::add_strarg(std::string s) {
	add_int(s.length());
	add_str(s);
}
void ResponseBuilder::add_bstrarg(char* s, long long len) {
	add_int(len);
	add_str(s, len);
}
void ResponseBuilder::padding(long long len) {
	while (input.size() %  len != 0)
	{
		add_byte(0);
	}
}
std::vector<UCHAR> ResponseBuilder::build() {
	std::vector<UCHAR> _temp;
	_temp.assign(input.begin(), input.end());
	input.clear();
	input.~vector();
	return _temp;
}