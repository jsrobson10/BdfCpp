
#include "Bdf.h"
#include "BdfHelpers.h"
#include <vector>
#include <cstdint>
#include <iostream>
#include <string.h>


BdfArray::BdfArray(BdfLookupTable* lookupTable, const char* data, int size)
{
	int i = 0;

	while(i < size)
	{
		char object_size_tag;
		BdfObject::getFlagData(data, NULL, &object_size_tag, NULL);

		char object_size_bytes = BdfObject::getSizeBytes(object_size_tag);
		
		if(i + object_size_bytes >= size) {
			return;
		}

		// Get the size of the object
		int object_size = BdfObject::getSize(data + i);
	
		if(object_size <= 0 || i + object_size > size) {
			return;
		}

		// Add the object to the elements list
		objects.push_back(new BdfObject(lookupTable, data + i, object_size));

		// Increase the iterator by the amount of bytes
		i += object_size;
	}
}

BdfArray::BdfArray(BdfLookupTable* lookupTable) : BdfArray(lookupTable, NULL, 0) {

}

BdfArray::BdfArray(BdfLookupTable* lookupTable, BdfStringReader* sr)
{
	sr->upto += 1;

	// [..., ...]

	try
	{	
		for(;;)
		{
			sr->ignoreBlanks();

			if(sr->upto[0] == ']') {
				sr->upto += 1;
				return;
			}

			BdfObject* bdf = new BdfObject(lookupTable, sr);
			add(bdf);

			// There should be a comma after this
			sr->ignoreBlanks();
			wchar_t c = sr->upto[0];
	
			if(c == ']') {
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

	catch(BdfError &e)
	{
		for(BdfObject* bdf : objects) {
			delete bdf;
		}

		throw;
	}
}

BdfArray::~BdfArray()
{
	for(BdfObject* bdf : objects) {
		delete bdf;
	}
}

int BdfArray::size() {
	return objects.size();
}

BdfObject* BdfArray::get(int index) {
	return objects[index];
}

BdfArray* BdfArray::set(int index, BdfObject* o) {
	objects[index] = o;
	return this;
}

BdfArray* BdfArray::add(BdfObject* o) {
	objects.push_back(o);
	return this;
}

BdfObject* BdfArray::remove(int index)
{
	BdfObject* o = objects[index];
	objects.erase(objects.begin() + index);
	return o;
}

BdfArray* BdfArray::clear() {
	objects.clear();
	return this;
}

void BdfArray::getLocationUses(int* locations)
{
	for(BdfObject* object : objects) {
		object->getLocationUses(locations);
	}
}

int BdfArray::serializeSeeker(int* locations)
{
	int size = 0;

	for(BdfObject* object : objects) {
		size += object->serializeSeeker(locations);
	}

	return size;
}

int BdfArray::serialize(char *data, int* locations)
{
	int pos = 0;

	for(BdfObject* object : objects) {
		pos += object->serialize(data + pos, locations, 0);
	}

	return pos;
}

void BdfArray::serializeHumanReadable(std::ostream &out, BdfIndent indent, int it)
{
	if(objects.size() == 0) {
		out << "[]";
		return;
	}

	out << "[";

	for(unsigned int i=0;i<objects.size();i++)
	{
		BdfObject *o = objects[i];

		out << indent.breaker;

		for(int n=0;n<=it;n++) {
			out << indent.indent;
		}

		o->serializeHumanReadable(out, indent, it + 1);

		if(objects.size() > i + 1) {
			out << ", ";
		}
	}

 	out << indent.breaker;

	for(int n=0;n<it;n++) {
		out << indent.indent;
	}

	out << "]";
}


