
#include "BdfStringReader.h"
#include "BdfError.h"

#include <iostream>
#include <string>
#include <codecvt>
#include <locale>

const std::string ERRORS[4] = {
	"Syntax error",
	"End of file",
	"Unescaped comment",
	"Unescaped string",
};

BdfError::BdfError(const int code, BdfStringReader reader)
{
	type = code;
		
	const wchar_t* start_of_line = reader.start;
	int line = 0;
	int at = 0;

	if(reader.upto > reader.end - 2) {
		reader.upto = reader.end - 2;
	}

	for(const wchar_t* i=reader.start;i<reader.upto;i++)
	{
		if(i[0] == '\n') {
			start_of_line = i + 1;
			line += 1;
			at = 0;
			continue;
		}

		at += 1;
	}

	int line_size = 0;
	std::string spacer = "";

	for(const wchar_t* i=start_of_line;i<reader.end;i++)
	{
		if(i[0] == '\n') {
			break;
		}

		line_size += 1;

		if(i == reader.end - 1) {
			break;
		}

		if(i < reader.upto)
		{
			if(i[0] == '\t') {
				spacer += "\t";
				continue;
			}

			spacer += " ";
		}
	}

	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
	
	error_short = ERRORS[code] + " " + std::to_string(line + 1) + ":" + std::to_string(at + 1);
	
	message  = error_short + "\n";
	message += cv.to_bytes(std::wstring(start_of_line, line_size)) + "\n";
	message += spacer;
	message += "^";
}

std::string BdfError::getErrorShort() {
	return error_short;
}

std::string BdfError::getError() {
	return message;
}

int BdfError::getType() {
	return type;
}

/*const char* BdfError::what() const throw() {
	return message.c_str();
}*/
