
#ifndef BDFHELPERS_HPP_
#define BDFHELPERS_HPP_

#include <string>

namespace BdfHelpers
{
	void reverseIfLittleEndian(void* to, const void* from, int size);
	
	std::string serializeString(std::string v);
}

#endif
