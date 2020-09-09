
#ifndef IS_LITTLE_ENDIAN
#define IS_LITTLE_ENDIAN 1
#endif

#include "BdfHelpers.h"
#include <string.h>
#include <iostream>
#include <codecvt>
#include <locale>

char hex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

void reverseArray(void* to, const void* from, int size)
{
	for(int i=0;i<size;i++) {
		*((char*)to + size - i - 1) = *((char*)from + i);
	}
}

void reverseIfLittleEndian(void* to, const void* from, int size)
{
	#if IS_LITTLE_ENDIAN == 1
	
	reverseArray(to, from, size);
	
	#elif IS_LITTLE_ENDIAN == 0

	memcpy(to, from, size);
	
	#endif
}

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

std::string serializeString(std::string str)
{
	// Try to convert to wide chars
	try
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;

		return cv.to_bytes(serializeWString(cv.from_bytes(str)));
	}

	catch(std::range_error e) {
	}

	catch(std::length_error e) {
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

std::string toHex(char* data, int size)
{
  std::string r;

  for(int i=0;i<size;i++) {
    char c = data[i];
    r += hex[c / 16];
    r += hex[c % 16];
    r += " ";
  }

  return r;
}
