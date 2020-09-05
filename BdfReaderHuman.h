
#ifndef BDFREADERHUMAN_H_
#define BDFREADERHUMAN_H_

#include <string>

class BdfReaderHuman : public BdfReader
{
public:
	BdfReaderHuman(std::string data);
	BdfReaderHuman(std::wstring data);
};

#endif
