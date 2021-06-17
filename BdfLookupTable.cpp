
#include "Bdf.hpp"
#include "BdfHelpers.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <string.h>

using namespace Bdf;
using namespace BdfHelpers;

BdfLookupTable::BdfLookupTable(BdfReader* pReader)
{
	reader = pReader;
	keys_mapped = NULL;
	keys_size_mapped = 0;
	keys_start = NULL;
	keys_endp = &keys_start;
	keys_size = 0;
}

BdfLookupTable::BdfLookupTable(BdfReader* pReader, const char* data, int size) : BdfLookupTable(pReader)
{
	for(int i=0;i<size;)
	{
		unsigned char key_size = *(unsigned char*)(&data[i]);

		i += 1;

		if(i + key_size > size) {
			return;
		}

		Item* key_new = new Item();
		key_new->key = std::string(data + i, key_size);
		key_new->next = NULL;

		*keys_endp = key_new;
		keys_endp = &key_new->next;

		keys_size += 1;
		i += key_size;
	}
}

BdfLookupTable::~BdfLookupTable()
{
	free(keys_mapped);

	Item* cur = keys_start;
	Item* next;

	while(cur != NULL)
	{
		next = cur->next;

		delete cur;

		cur = next;
	}
}

void BdfLookupTable::remapKeys()
{
	if(keys_size != keys_size_mapped)
	{
		keys_mapped = (Item**)realloc(keys_mapped, keys_size * sizeof(Item*));
	}

	keys_size_mapped = keys_size;

	Item* cur = keys_start;
	int upto = 0;

	while(cur != NULL)
	{
		keys_mapped[upto] = cur;
		cur = cur->next;
		upto += 1;
	}
}

unsigned int BdfLookupTable::getLocation(std::string key)
{
	Item* cur = keys_start;
	int upto = 0;

	while(cur != NULL)
	{
		if(cur->key == key)
		{
			return upto;
		}
			
		cur = cur->next;
		upto += 1;
	}

	Item* item = new Item();

	item->key = key;
	item->next = NULL;

	*this->keys_endp = item;
	this->keys_endp = &item->next;

	return keys_size++;
}

std::string BdfLookupTable::getName(unsigned int key)
{
	if(keys_size != keys_size_mapped) remapKeys();
	if(key < 0 || key >= keys_size) return "";

	return keys_mapped[key]->key;
}

bool BdfLookupTable::hasKeyLocation(unsigned int key) {
	return key >= 0 && key < keys_size;
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

		std::string key = keys_mapped[i]->key;

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

		size += keys_mapped[i]->key.size() + 1;
	}

	return size;
}

void BdfLookupTable::serializeGetLocations(int* locations)
{
	if(keys_size != keys_size_mapped) remapKeys();
	
	int* uses = new int[keys_size];
	int next = 0;

	// Fill the uses array with zeros
	for(unsigned int i=0;i<keys_size;i++) {
		uses[i] = 0;
	}	

	reader->getObject()->getLocationUses(uses);

	for(unsigned int i=0;i<keys_size;i++)
	{
		if(uses[i] > 0) {
			locations[i] = next;
			next += 1;
		} else {
			locations[i] = -1;
		}
	}

	delete[] uses;
}

int BdfLookupTable::size() {
	return keys_size;
}
