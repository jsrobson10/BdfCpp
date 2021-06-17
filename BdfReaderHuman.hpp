
#ifndef BDFREADERHUMAN_HPP_
#define BDFREADERHUMAN_HPP_

#include <string>

namespace Bdf
{
	class BdfReaderHuman : public BdfReader
	{
	public:
		BdfReaderHuman(std::string data);
		BdfReaderHuman(std::wstring data);
	};
}

#endif
