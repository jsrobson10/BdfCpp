
#ifndef BDFLOOKUPTABLE_H_
#define BDFLOOKUPTABLE_H_

#include "Bdf.h"
#include <iostream>
#include <vector>
#include <string>

class BdfLookupTable
{
private:
	std::vector<std::string> keys;
	BdfReader* reader;

public:
	BdfLookupTable(BdfReader* reader);
	BdfLookupTable(BdfReader* reader, const char* data, int size);
	virtual ~BdfLookupTable();
	unsigned int getLocation(std::string id);
	std::string getName(unsigned int id);
	int serialize(char* database, int* locations, int locations_size);
	int serializeSeeker(int* locations, int locations_size);
	void serializeGetLocations(int* locations);
	bool hasKeyLocation(unsigned int key);
	int size();
};

#endif
