
#include "Bdf.h"
#include "BdfHelpers.h"
#include <cstdint>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>

BdfNamedListObject::~BdfNamedListObject() {
}

BdfNamedListObject::BdfNamedListObject(int pKey, BdfObject* pObject)
{
	object = pObject;
	key = pKey;
}

BdfNamedList::BdfNamedList(BdfLookupTable* pLookupTable, const char* data, int size)
{
	lookupTable = pLookupTable;

	int i = 0;

	while(i < size)
	{
		// Get the object
		char key_size = 0;
		char key_size_bytes;
		char bdf_size = 0;
		char bdf_size_bytes;
		
		BdfObject::getFlagData(data + i, NULL, &bdf_size_bytes, &key_size_bytes);
		key_size = BdfObject::getSizeBytes(key_size_bytes);

		if(i + bdf_size >= size) {
			return;
		}

		int object_size = BdfObject::getSize(data + i);
		const char* object_data = data + i;

		if(object_size <= 0 || i + object_size > size) {
			return;
		}

		i += object_size;

		// Get the key
		int key = 0;
		char key_buff[key_size];
		reverseIfLittleEndian(key_buff, data + i, key_size);

		switch(key_size_bytes)
		{
			case 2:
				key = *((unsigned char*)key_buff);
				break;
			case 1:
				key = *((unsigned short*)key_buff);
				break;
			case 0:
				key = *((int*)key_buff);
				break;
		}

		if(lookupTable->hasKeyLocation(key)) {
			return;
		}

		i += key_size;

		// Add the list item
		objects.push_back(BdfNamedListObject(key, new BdfObject(lookupTable, object_data, object_size)));
	}
}

BdfNamedList::BdfNamedList(BdfLookupTable* lookupTable) : BdfNamedList(lookupTable, NULL, 0) {

}

BdfNamedList::BdfNamedList(BdfLookupTable* pLookupTable, BdfStringReader* sr)
{
	lookupTable = pLookupTable;
	sr->upto += 1;

	// {"key": ..., "key2": ...}
	try
	{
		for(;;)
		{
			sr->ignoreBlanks();
	
			wchar_t c = sr->upto[0];
	
			if(c == '}') {
				sr->upto += 1;
				break;
			}
	
			if(c != '"') {
				throw BdfError(BdfError::ERROR_SYNTAX, *sr);
			}
	
			std::string key = sr->getQuotedString();
	
			// There should be a colon after this
			sr->ignoreBlanks();
			if(sr->upto[0] != ':') {
				std::cout << "ERROR: " << sr->upto[0] << "\n";
				throw BdfError(BdfError::ERROR_SYNTAX, *sr);
			}
	
			sr->upto += 1;
			sr->ignoreBlanks();
	
			BdfObject* bdf = new BdfObject(lookupTable, sr);
			set(key, bdf);
	
			// There should be a comma after this
			sr->ignoreBlanks();
			c = sr->upto[0];
	
			if(c == '}') {
				sr->upto += 1;
				return;
			}
	
			if(c != ',') {
				throw BdfError(BdfError::ERROR_SYNTAX, *sr);
			}
	
			sr->upto += 1;
			sr->ignoreBlanks();
		}
	}

	catch(BdfError e)
	{
		for(BdfNamedListObject o : objects) {
			delete o.object;
		}

		throw;
	}
}

BdfNamedList::~BdfNamedList()
{
	for(BdfNamedListObject o : objects) {
		delete o.object;
	}
}

std::vector<int> BdfNamedList::keys()
{
	std::vector<int> keys;
	keys.resize(objects.size());

	for(unsigned int i=0;i<objects.size();i++) {
		keys.push_back(objects[i].key);
	}

	return keys;
}

bool BdfNamedList::exists(std::string key) {
	return exists(lookupTable->getLocation(key));
}

bool BdfNamedList::exists(int key)
{
	for(unsigned int i=0;i<objects.size();i++) {
		if(objects[i].key == key) {
			return true;
		}
	}

	return false;
}

BdfNamedList* BdfNamedList::set(std::string key, BdfObject* v) {
	return set(lookupTable->getLocation(key), v);
}

BdfNamedList* BdfNamedList::set(int key, BdfObject* v)
{
	for(unsigned int i=0;i<objects.size();i++) {
		if(objects[i].key == key) {
			objects[i].object = v;
			return this;
		}
	}

	objects.push_back(BdfNamedListObject(key, v));

	return this;
}

BdfObject* BdfNamedList::remove(std::string key) {
	return remove(lookupTable->getLocation(key));
}

BdfObject* BdfNamedList::remove(int key)
{
	for(unsigned int i=0;i<objects.size();i++) {
		if(objects[i].key == key)
		{
			BdfObject* v = objects[i].object;
			objects.erase(objects.begin() + i);
			return v;
		}
	}

	return new BdfObject(lookupTable);
}

BdfObject* BdfNamedList::get(std::string key) {
	return get(lookupTable->getLocation(key));
}

BdfObject* BdfNamedList::get(int key)
{
	for(unsigned int i=0;i<objects.size();i++) {
		if(objects[i].key == key) {
			return objects[i].object;
		}
	}

	BdfObject* v = new BdfObject(lookupTable);
	set(key, v);

	return v;
}

int BdfNamedList::serializeSeeker(int* locations)
{
	int size = 0;

	for(BdfNamedListObject object : objects)
	{
		int location = locations[object.key];

		if(location > 65535) {		// >= 2 ^ 16
			size += 4;
		} else if(location > 255) {	// >= 2 ^ 8
			size += 2;
		} else {					// < 2 ^ 8
			size += 1;
		}

		size += object.object->serializeSeeker(locations);
	}

	return size;
}

int BdfNamedList::serialize(char* data, int* locations)
{
	int pos = 0;

	for(BdfNamedListObject object : objects)
	{
		int location = locations[object.key];

		char size_bytes_tag;
		char size_bytes;

		if(location > 65535) {
			size_bytes_tag = 0;
			size_bytes = 4;
		} else if(location > 255) {
			size_bytes_tag = 1;
			size_bytes = 2;
		} else {
			size_bytes_tag = 2;
			size_bytes = 1;
		}

		int size = object.object->serialize(data + pos, locations, size_bytes_tag);
		int offset = pos + size;

		char bytes[4];
		reverseIfLittleEndian(bytes, &location, 4);

		for(int i=0;i<size_bytes;i++) {
			data[i + offset] = bytes[i - size_bytes + 4];
		}

		pos += size + size_bytes;
	}

	return pos;
}

void BdfNamedList::serializeHumanReadable(std::ostream &out, BdfIndent indent, int it)
{
	if(objects.size() == 0) {
		out << "{}";
		return;
	}

	out << "{";

	for(unsigned int i=0;i<objects.size();i++)
	{
		BdfNamedListObject list_o = objects[i];

		out << indent.breaker;

		for(int n=0;n<=it;n++) {
			out << indent.indent;
		}
		
		std::string name = lookupTable->getName(list_o.key);

		out << serializeString(name) << ": ";
		list_o.object->serializeHumanReadable(out, indent, it + 1);

		if(objects.size() > i + 1) {
			out << ", ";
		}
	}

 	out << indent.breaker;

	for(int n=0;n<it;n++) {
		out << indent.indent;
	}

	out << "}";
}

void BdfNamedList::getLocationUses(int* locations)
{
	for(BdfNamedListObject object : objects) {
		locations[object.key] += 1;
		object.object->getLocationUses(locations);
	}
}
