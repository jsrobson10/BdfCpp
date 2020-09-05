
#include "Bdf.h"
#include "BdfHelpers.h"
#include <iostream>
#include <vector>
#include <string>
#include <string.h>

BdfLookupTable::BdfLookupTable(BdfReader* pReader) {
	reader = pReader;
}

BdfLookupTable::BdfLookupTable(BdfReader* pReader, const char* data, int size)
{
	reader = pReader;

	for(int i=0;i<size;)
	{
		unsigned char key_size = *(unsigned char*)(&data[i]);

		i += 1;

		if(i + key_size > size) {
			return;
		}

		keys.push_back(std::string(data + i, key_size));

		i += key_size;
	}
}

BdfLookupTable::~BdfLookupTable()
{

}

unsigned int BdfLookupTable::getLocation(std::string key)
{
	for(unsigned int i=0;i<keys.size();i++)
	{
		if(keys[i] == key) {
			return i;
		}
	}

	keys.push_back(key);
	return keys.size() - 1;
}

std::string BdfLookupTable::getName(unsigned int key) {
	return keys[key];
}

bool BdfLookupTable::hasKeyLocation(unsigned int key) {
	return key < 0 || key >= keys.size();
}

int BdfLookupTable::serialize(char* data, int* locations, int locations_size)
{
	int upto = 0;

	for(int i=0;i<locations_size;i++)
	{
		int loc = locations[i];

		if(loc == -1) {
			continue;
		}

		std::string key = keys[i];

		memcpy(data + upto + 1, key.c_str(), key.size());
		data[upto] = (char) key.size();

		upto += key.size() + 1;
	}

	return upto;
}

int BdfLookupTable::serializeSeeker(int* locations, int locations_size)
{
	int size = 0;

	for(int i=0;i<locations_size;i++)
	{
		// Skip this key if the location is unset (the key has been culled)
		int loc = locations[i];

		if(loc == -1) {
			continue;
		}

		size += keys[i].size() + 1;
	}

	return size;
}

void BdfLookupTable::serializeGetLocations(int* locations)
{
	int size = keys.size();
	int uses[size];
	int next = 0;

	// Fill the uses array with zeros
	for(int i=0;i<size;i++) {
		uses[i] = 0;
	}	

	reader->getObject()->getLocationUses(uses);

	for(int i=0;i<size;i++)
	{
		if(uses[i] > 0) {
			locations[i] = next;
			next += 1;
		} else {
			locations[i] = -1;
		}
	}
}

int BdfLookupTable::size() {
	return keys.size();
}
