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
		char* bytes_data = new char[size_data];
		memcpy(bytes_data, pData + i, size_data);

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

int BdfArray::serialize(char **pData)
{
	std::vector<char> data;

	for(BdfObject *object : objects)
	{
		char *object_data;
		int32_t size = object->serialize(&object_data);
		char *object_size = reverseIfLittleEndian((char*)&size, sizeof(size));

		for(int i=0;i<sizeof(int32_t);i++) {
			data.push_back(object_size[i]);
		}

		for(int i=0;i<size;i++) {
			data.push_back(object_data[i]);
		}

		free(object_data);
		free(object_size);
	}

	char *data2 = new char[data.size()];

	for(int i=0;i<data.size();i++) {
		data2[i] = data[i];
	}

	*pData = data2;
	return data.size();
}

std::string BdfArray::serializeHumanReadable(BdfIndent indent, int it)
{
	if(objects.size() == 0) {
		return "[]";
	}

	std::string data = "[";

	for(int i=0;i<objects.size();i++)
	{
		BdfObject *o = objects[i];

		data += indent.breaker;

		for(int n=0;n<=it;n++) {
			data += indent.indent;
		}

		data += o->serializeHumanReadable(indent, it + 1);

		if(objects.size() > i + 1) {
			data += ", ";
		}
	}

	data += indent.breaker;

	for(int n=0;n<it;n++) {
		data += indent.indent;
	}

	return data + "]";
}
