
#ifndef BDFINDENT_HPP_
#define BDFINDENT_HPP_

#include <string>

namespace Bdf
{
	class BdfIndent
	{
	public:
	
		std::string indent;
		std::string breaker;
	
		BdfIndent(std::string indent, std::string breaker);
	};
}

#endif
