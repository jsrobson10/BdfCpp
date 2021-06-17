
#include "BdfStringReader.hpp"
#include "BdfError.hpp"

#include <string>
#include <codecvt>
#include <locale>

using namespace Bdf;

BdfStringReader::BdfStringReader(const wchar_t* ptr, int size)
{
	start = ptr;
	upto = ptr;
	end = ptr + size;
}

int BdfStringReader::length() {
	return end - start;
}

BdfStringReader BdfStringReader::getPointer(int offset) {
	BdfStringReader other(start, end - start);
	other.upto = upto + offset;
	return other;
}

void BdfStringReader::ignoreBlanks()
{
	for(;;)
	{
		checkRange();
		
		wchar_t c = upto[0];

		// Comments
		if(c == '/' && upto < end)
		{
			wchar_t c2 = upto[1];

			// Line comment
			if(c2 == '/')
			{
				for(;;)
				{
					if(!inRange()) {
						break;
					}

					upto += 1;
					c = upto[0];

					if(c == '\n') {
						break;
					}
				}
			}
	
			// Multi-line comment
			else if(c2 == '*')
			{
				for(;;)
				{
					if(!inRange()) {
						throw BdfError(BdfError::ERROR_UNESCAPED_COMMENT, *this);
					}

					upto += 1;
					c = upto[0];

					if(c == '*' && upto < end && upto[1] == '/') {
						upto += 1;
						break;
					}
				}
			}

			else {
				return;
			}
		}

		else if(!(c == '\n' || c == '\t' || c == ' ')) {
			return;
		}

		upto += 1;
	}
}

std::string BdfStringReader::getQuotedString()
{
	if(upto[0] != '"') {
		throw BdfError(BdfError::ERROR_SYNTAX, *this);
	}

	upto += 1;
	std::wstring str = L"";

	for(;;)
	{
		if(!inRange()) {
			throw BdfError(BdfError::ERROR_UNESCAPED_STRING, *this);
		}
		
		wchar_t c = upto[0];
		
		// Check for back slashes
		if(c == '\\')
		{
			upto += 1;
			c = upto[0];

			switch(c)
			{
				case 'n':
					str += L"\n";
					upto += 1;
					break;
				case 't':
					str += L"\t";
					upto += 1;
					break;
				case '\\':
					str += L"\\";
					upto += 1;
					break;
				case '\n':
					str += L"\n";
					upto += 1;
					break;
				case 'u': // \u0000
				{
					if(upto + 5 >= end) {
						throw BdfError(BdfError::ERROR_UNESCAPED_STRING, getPointer(1));
					}

					wchar_t unicode = 0;
					int m = 1;

					for(int j=3;j>=0;j--)
					{
						c = upto[j];

						if(c >= '0' && c <= '9') {
							unicode += m * (c - '0');
						}

						else if(c >= 'a' && c <= 'f') {
							unicode += m * (c - 'a' + 10);
						}

						else {
							throw BdfError(BdfError::ERROR_SYNTAX, getPointer(4 - j));
						}

						m *= 16;
					}

					str += unicode;
					upto += 5;

					break;
				}

				default:
					str += L"\\" + c;
			}
		}

		else if(c == '"') {
			upto += 1;
			break;
		}

		else {
			upto += 1;
			str += c;
		}
	}

	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;

	return cv.to_bytes(str);
}

void BdfStringReader::checkRange()
{
	if(upto >= end) {
		throw BdfError(BdfError::ERROR_END_OF_FILE, *this);
	}
}

bool BdfStringReader::inRange() {
	return upto < end;
}

bool BdfStringReader::isNext(std::wstring check)
{
	if(check.size() + upto > end) {
		return false;
	}

	for(unsigned int i=0;i<check.size();i++)
	{
		wchar_t c = upto[i];
		c = (c >= 'A' && c <= 'Z') ? (c + 32) : c;

		if(c != check[i]) {
			return false;
		}
	}

	upto += check.size();
	
	return true;
}
