/*
 * BdfArray.cpp
 *
 *  Created on: 31 May 2020
 *      Author: josua
 */

#include "headers.h"
#include "helpers.h"
#include <vector>
#include <cstdint>
#include <iostream>
#include <string.h>


BdfArray::BdfArray(char pData[], int pSize)
{
	int i = 0;

	while(true)
	{
		if(i+4>pSize) {
			break;
		}

		// Get the size of the data
		char* size_data_c = reverseIfLittleEndian(pData + i, 4);
		int32_t size_data = *((int32_t*)size_data_c);
		delete[] size_data_c;

		// Move along memory
		i += 4;

		if(i+size_data > pSize) {
			break;
		}

		// Get the data
		char* bytes_data = pData + i;

		// Move along memory
		i += size_data;

		// Add the bdf object
		objects.push_back(new BdfObject(bytes_data, size_data));
	}
}

BdfArray::BdfArray() : BdfArray(new char[0], 0) {

}

BdfArray::~BdfArray() {
	// TODO Auto-generated destructor stub
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

int BdfArray::_serializeSeek()
{
	int size = 0;

	for(BdfObject* object : objects) {
		size += object->_serializeSeek();
		size += 4;
	}

	return size;
}

int BdfArray::_serialize(char *data)
{
	int size = 0;

	for(BdfObject* object : objects)
	{
		// Get the object
		int32_t size_object = object->_serialize(data + size + 4);
		char* size_object_c = reverseIfLittleEndian(&size_object, 4);

		// Send back the object
		memcpy(data + size, size_object_c, 4);
		delete[] size_object_c;
		size += size_object;
		size += 4;
	}

	return size;
}

void BdfArray::serializeHumanReadable(std::ostream &out, BdfIndent indent, int it)
{
	if(objects.size() == 0) {
		out << "[]";
		return;
	}

	out << "[";

	for(int i=0;i<objects.size();i++)
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

void BdfArray::freeAll()
{
	for(BdfObject* bdf : objects) {
		bdf->freeAll();
	}
}
