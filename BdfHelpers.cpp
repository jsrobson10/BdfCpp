
#include "BdfHelpers.hpp"
#include <string.h>
#include <iostream>
#include <codecvt>
#include <locale>

char hex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

std::string replaceInString(std::string str, char find, std::string replace)
{
	std::string str_mod = "";

	for(unsigned int i=0;i<str.size();i++)
	{
		if(find == str[i]) {
			str_mod += replace;
		}

		else {
			str_mod += str[i];
		}
	}

	return str_mod;
}

std::wstring serializeWString(std::wstring str)
{
	std::wstring str_new = L"\"";

	for(unsigned int i=0;i<str.size();i++)
	{
		wchar_t c = str[i];
		
		switch(c)
		{
			case '\n':
				str_new += L"\\n";
				continue;
			case '\t':
				str_new += L"\\t";
				continue;
			case '\r':
				str_new += L"\\r";
				continue;
			case '\\':
				str_new += L"\\\\";
				continue;
			case '"':
				str_new += L"\\\"";
				continue;
		}
		
		if(c < 0x20 || (c > 0x7e && c < 0xa1) || c == 0xad)
		{
			// Will the in the format \u0000
			str_new += L"\\u";
			str_new += hex[(c & 0xf000) >> 12];
			str_new += hex[(c & 0x0f00) >> 8];
			str_new += hex[(c & 0x00f0) >> 4];
			str_new += hex[(c & 0x000f)];
			continue;
		}

		str_new += c;
	}

	return str_new + L"\"";
}

std::string BdfHelpers::serializeString(std::string str)
{
	// Try to convert to wide chars
	try
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;

		return cv.to_bytes(serializeWString(cv.from_bytes(str)));
	}

	catch(std::range_error &e) {
	}

	catch(std::length_error &e) {
	}

	// Default to utf-8
	
	std::string str_new = "\"";

	for(unsigned int i=0;i<str.size();i++)
	{
		char c = str[i];
		
		switch(c)
		{
			case '\n':
				str_new += "\\n";
				continue;
			case '\t':
				str_new += "\\t";
				continue;
			case '\r':
				str_new += "\\r";
				continue;
			case '\\':
				str_new += "\\\\";
				continue;
			case '"':
				str_new += "\\\"";
				continue;
		}
		
		if(c < 0x20 || (c > 0x7e && c < 0xa1) || c == 0xad)
		{
			// Will the in the format \u0000
			str_new += "\\u";
			str_new += hex[(c & 0xf000) >> 12];
			str_new += hex[(c & 0x0f00) >> 8];
			str_new += hex[(c & 0x00f0) >> 4];
			str_new += hex[(c & 0x000f)];
			continue;
		}

		str_new += c;
	}

	return str_new + "\"";
}

void BdfHelpers::put_netul(char* data, uint64_t num)
{
	data[0] = (num >> 56) & 255;
	data[1] = (num >> 48) & 255;
	data[2] = (num >> 40) & 255;
	data[3] = (num >> 32) & 255;
	data[4] = (num >> 24) & 255;
	data[5] = (num >> 16) & 255;
	data[6] = (num >> 8) & 255;
	data[7] = num & 255;
}

void BdfHelpers::put_netui(char* data, uint32_t num)
{
	data[0] = (num >> 24) & 255;
	data[1] = (num >> 16) & 255;
	data[2] = (num >> 8) & 255;
	data[3] = num & 255;
}

void BdfHelpers::put_netus(char* data, uint16_t num)
{
	data[0] = (num >> 8) & 255;
	data[1] = num & 255;
}

void BdfHelpers::put_netsl(char* data, int64_t num)
{
	put_netul(data, (uint64_t)num);
}

void BdfHelpers::put_netsi(char* data, int32_t num)
{
	put_netui(data, (uint32_t)num);
}

void BdfHelpers::put_netss(char* data, int16_t num)
{
	put_netus(data, (uint16_t)num);
}

void BdfHelpers::put_netf(char* data, float num)
{
	uint32_t num_int = *(uint32_t*)&num;

	put_netui(data, num_int);
}

void BdfHelpers::put_netd(char* data, double num)
{
	uint64_t num_int = *(uint64_t*)&num;

	put_netul(data, num_int);
}

uint64_t BdfHelpers::get_netul(const char* data)
{
	return (((uint64_t)data[0] & 255) << 56) ^ (((uint64_t)data[1] & 255) << 48) ^ (((uint64_t)data[2] & 255) << 40) ^ (((uint64_t)data[3] & 255) << 32) ^
			(((uint64_t)data[4] & 255) << 24) ^ (((uint64_t)data[5] & 255) << 16) ^ (((uint64_t)data[6] & 255) << 8) ^ ((uint64_t)data[7] & 255);
}

uint32_t BdfHelpers::get_netui(const char* data)
{
	return (((uint32_t)data[0] & 255) << 24) ^ (((uint32_t)data[1] & 255) << 16) ^ (((uint32_t)data[2] & 255) << 8) ^ ((uint32_t)data[3] & 255);
}

uint16_t BdfHelpers::get_netus(const char* data)
{
	return (((uint16_t)data[0] & 255) << 8) ^ ((uint16_t)data[1] & 255);
}

int64_t BdfHelpers::get_netsl(const char* data)
{
	return (int64_t)get_netul(data);
}

int32_t BdfHelpers::get_netsi(const char* data)
{
	return (int32_t)get_netui(data);
}

int16_t BdfHelpers::get_netss(const char* data)
{
	return (int16_t)get_netus(data);
}

float BdfHelpers::get_netf(const char* data)
{
	uint32_t num = get_netui(data);

	return *(float*)&num;
}

double BdfHelpers::get_netd(const char* data)
{
	uint64_t num = get_netul(data);

	return *(double*)&num;
}

