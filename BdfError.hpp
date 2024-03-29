
#ifndef BDFERROR_HPP_
#define BDFERROR_HPP_

#include "BdfStringReader.hpp"
#include <string>

namespace Bdf
{
	class BdfError : public std::exception
	{
	private:
		std::string error_short;
		std::string message;
		int type;
	
	public:
	
		static const int ERROR_SYNTAX = 0;
		static const int ERROR_END_OF_FILE = 1;
		static const int ERROR_UNESCAPED_COMMENT = 2;
		static const int ERROR_UNESCAPED_STRING = 3;
		static const int ERROR_OUT_OF_RANGE = 4;
	
		BdfError(const int code, BdfStringReader reader, int length);
		BdfError(const int code, BdfStringReader reader);
		std::string getErrorShort();
		std::string getError();
		int getType();
	
		virtual const char* what() const throw();
	};
}

#endif
