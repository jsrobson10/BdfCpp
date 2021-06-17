
#ifndef BDFTYPES_HPP_
#define BDFTYPES_HPP_

namespace Bdf
{
	class BdfTypes
	{
		public:
	
		const static char UNDEFINED = 0;
	
		const static char BOOLEAN = 1;
		const static char INTEGER = 2;
		const static char LONG = 3;
		const static char SHORT = 4;
		const static char BYTE = 5;
		const static char DOUBLE = 6;
		const static char FLOAT = 7;
	
		const static char STRING = 8;
		const static char LIST = 9;
		const static char NAMED_LIST = 10;
	
		const static char ARRAY_BOOLEAN = 11;
		const static char ARRAY_INTEGER = 12;
		const static char ARRAY_LONG = 13;
		const static char ARRAY_SHORT = 14;
		const static char ARRAY_BYTE = 15;
		const static char ARRAY_DOUBLE = 16;
		const static char ARRAY_FLOAT = 17;
	};
}

#endif
