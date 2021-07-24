
#include "Bdf.hpp"
#include "BdfHelpers.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <codecvt>
#include <locale>

using namespace Bdf;
using namespace BdfHelpers;

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
	lookupTable_size_bytes = BdfObject::getSizeBytes(lookupTable_size_tag);
	bdf_size_bytes = BdfObject::getSizeBytes(bdf_size_tag);	
	
	// Check if there is enough space
	if(1 + lookupTable_size_bytes + bdf_size_bytes > size) {
		initEmpty();
		return;
	}
	
	// Get the rest of the data
	int bdf_size = BdfObject::getSize(data);
	
	// Check if there is enough space in the buffer
	if(bdf_size <= 0 || bdf_size + lookupTable_size_bytes > size) {
		initEmpty();
		return;
	}
	
	const char* data_bdf = data;
	data += bdf_size;

	// Get the size of the lookup table
	int lookupTable_size = 0;

	switch(lookupTable_size_tag)
	{
		case 0:
			lookupTable_size = get_netsi(data);
			break;
		case 1:
			lookupTable_size = get_netus(data);
			break;
		case 2:
			lookupTable_size = data[0] & 255;
			break;
	}
	
	// Check if there is enough space in the buffer
	if(bdf_size + lookupTable_size_bytes + lookupTable_size > size) {
		initEmpty();
		return;
	}
	
	// Load the lookup table and the objects from the buffer
	lookupTable = new BdfLookupTable(this, data + lookupTable_size_bytes, lookupTable_size);
	bdf = new BdfObject(lookupTable, data_bdf, bdf_size);
}

BdfReader::~BdfReader() {
	delete lookupTable;
	delete bdf;
}

void BdfReader::serialize(char** pData, int* pSize)
{
	int locations_size = lookupTable->size();
	int* locations = new int[locations_size];

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

	switch(lookupTable_size_bytes)
	{
		case 4:
			put_netsi(data, lookupTable_size);
			break;
		case 2:
			put_netus(data, lookupTable_size);
			break;
		default:
			data[0] = lookupTable_size & 255;
	}

	lookupTable->serialize(data + lookupTable_size_bytes, locations, locations_size);

	delete[] locations;
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
