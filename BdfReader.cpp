
#include "Bdf.h"
#include "BdfHelpers.h"
#include <iostream>
#include <string>
#include <sstream>
#include <codecvt>
#include <locale>


void BdfReader::initEmpty()
{
	lookupTable = new BdfLookupTable(this);
	bdf = new BdfObject(lookupTable);
}

BdfReader::BdfReader() {
	initEmpty();
}

BdfReader::BdfReader(const char* data, int size)
{
	if(size == 0) {
		initEmpty();
		return;
	}

	// Get the size of the bdf size tag and the lookup table size tag
	char lookupTable_size_tag;
	char lookupTable_size_bytes = 0;
	char bdf_size_tag;
	char bdf_size_bytes = 0;

	BdfObject::getFlagData(data, NULL, &bdf_size_tag, &lookupTable_size_tag);
	bdf_size_bytes = BdfObject::getSizeBytes(bdf_size_tag);

	switch(lookupTable_size_tag)
	{
		case 0:
			lookupTable_size_bytes = 4;
			break;
		case 1:
			lookupTable_size_bytes = 2;
			break;
		case 2:
			lookupTable_size_bytes = 1;
			break;
		default:
			initEmpty();
			return;
	}
	
	// Check if there is enough space
	if(1 + lookupTable_size_bytes + bdf_size_bytes > size) {
		initEmpty();
		return;
	}
	
	// Get the rest of the data
	int bdf_size = BdfObject::getSize(data);
	
	// Check if there is enough space in the buffer
	if(bdf_size < 0 || bdf_size + lookupTable_size_bytes > size) {
		initEmpty();
		return;
	}
	
	const char* data_bdf = data;
	data += bdf_size;

	// Get the size of the lookup table
	char* lookupTable_size_buff = new char[lookupTable_size_bytes];
	reverseIfLittleEndian(lookupTable_size_buff, data, lookupTable_size_bytes);
	int lookupTable_size = 0;

	switch(lookupTable_size_tag)
	{
		case 0:
			lookupTable_size = *((int*)lookupTable_size_buff);
			break;
		case 1:
			lookupTable_size = *((unsigned short*)lookupTable_size_buff);
			break;
		case 2:
			lookupTable_size = *((unsigned char*)lookupTable_size_buff);
			break;
	}
	
	// Check if there is enough space in the buffer
	if(bdf_size + lookupTable_size_bytes + lookupTable_size > size) {
		delete[] lookupTable_size_buff;
		initEmpty();
		return;
	}
	
	// Load the lookup table and the objects from the buffer
	lookupTable = new BdfLookupTable(this, data + lookupTable_size_bytes, lookupTable_size);
	bdf = new BdfObject(lookupTable, data_bdf, bdf_size);
	
	delete[] lookupTable_size_buff;
}

BdfReader::~BdfReader() {
	delete lookupTable;
	delete bdf;
}

void BdfReader::serialize(char** pData, int* pSize)
{
	int locations_size = lookupTable->size();
	int locations[locations_size];

	lookupTable->serializeGetLocations(locations);

	int bdf_size = bdf->serializeSeeker(locations);
	int lookupTable_size = lookupTable->serializeSeeker(locations, locations_size);

	int lookupTable_size_bytes = 0;
	char lookupTable_size_tag = 0;

	if(lookupTable_size > 65535) {
		lookupTable_size_tag = 0;
		lookupTable_size_bytes = 4;
	} else if(lookupTable_size > 255) {
		lookupTable_size_tag = 1;
		lookupTable_size_bytes = 2;
	} else {
		lookupTable_size_tag = 2;
		lookupTable_size_bytes = 1;
	}

	int data_size = bdf_size + lookupTable_size + lookupTable_size_bytes;
	char* data = new char[data_size];

	*pData = data;
	*pSize = data_size;
	
	bdf->serialize(data, locations, lookupTable_size_tag);
	data += bdf_size;

	char bytes[4];
	reverseIfLittleEndian(bytes, &lookupTable_size, 4);

	for(int i=0;i<lookupTable_size_bytes;i++) {
		data[i] = bytes[i - lookupTable_size_bytes + 4];
	}

	lookupTable->serialize(data + lookupTable_size_bytes, locations, locations_size);
}

BdfObject* BdfReader::getObject() {
	return bdf;
}

BdfObject* BdfReader::resetObject()
{
	delete bdf;
	bdf = new BdfObject(lookupTable);
	return bdf;
}

std::string BdfReader::serializeHumanReadable(BdfIndent indent)
{
	std::stringstream stream;

	bdf->serializeHumanReadable(stream, indent, 0);

	return stream.str();
}

std::string BdfReader::serializeHumanReadable() {
	return serializeHumanReadable(BdfIndent("", ""));
}

void BdfReader::serializeHumanReadable(std::ostream &stream) {
	bdf->serializeHumanReadable(stream, BdfIndent("", ""), 0);
	stream << "\n";
}

void BdfReader::serializeHumanReadable(std::ostream &stream, BdfIndent indent) {
	bdf->serializeHumanReadable(stream, indent, 0);
	stream << "\n";
}
