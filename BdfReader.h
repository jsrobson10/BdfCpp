
#ifndef BDFREADER_H_
#define BDFREADER_H_

#include "Bdf.h"
#include <iostream>
#include <string>

class BdfReader
{
protected:
	BdfObject* bdf;
	BdfLookupTable* lookupTable;
	void initEmpty();

public:
	BdfReader();
	BdfReader(const char* database, int size);
	virtual ~BdfReader();
	void serialize(char** data, int* size);
	BdfObject* getObject();
	BdfObject* resetObject();
	std::string serializeHumanReadable(BdfIndent indent);
	std::string serializeHumanReadable();
	void serializeHumanReadable(std::ostream &stream);
	void serializeHumanReadable(std::ostream &stream, BdfIndent indent);  
};

#endif
