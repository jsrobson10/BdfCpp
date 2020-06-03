/*
 * BdfNamedList.cpp
 *
 *  Created on: 31 May 2020
 *      Author: josua
 */

#include "headers.h"
#include "helpers.h"
#include <cstdint>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>


ListObject::ListObject(std::string pKey, BdfObject* pObject) {
	key = pKey;
	object = pObject;
}

ListObject::~ListObject() {

}

BdfNamedList::BdfNamedList(char* pData, int pSize)
{
	int i = 0;

	while(true)
	{
		if(i+4 > pSize) {
			break;
		}

		// Get the size of the key
		char* size_key_c = reverseIfLittleEndian(pData + i, 4);
		int32_t size_key = *((int32_t*)size_key_c);
		delete[] size_key_c;

		// Move along memory
		i += 4;

		if(i+size_key > pSize) {
			break;
		}

		// Get the key
		char* bytes_key = new char[size_key];
		memcpy(bytes_key, pData + i, size_key);

		// Move along memory
		i += size_key;

		if(i+4 > pSize) {
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

		// Add the new object
		objects.push_back(ListObject(std::string(bytes_key, size_key), new BdfObject(bytes_data, size_data)));


	}
}

BdfNamedList::BdfNamedList() : BdfNamedList(new char[0], 0) {

}

BdfNamedList::~BdfNamedList() {

}

std::vector<std::string> BdfNamedList::keys()
{
	std::vector<std::string> keys;
	keys.resize(objects.size());

	for(int i=0;i<objects.size();i++) {
		if(objects[i].object->getType() != BdfTypes::EMPTY) {
			keys.push_back(objects[i].key);
		}
	}

	return keys;
}

bool BdfNamedList::exists(std::string key)
{
	for(int i=0;i<objects.size();i++) {
		if(objects[i].key == key) {
			return objects[i].object->getType() != BdfTypes::EMPTY;
		}
	}

	return false;
}

BdfNamedList* BdfNamedList::set(std::string key, BdfObject* v)
{
	for(int i=0;i<objects.size();i++) {
		if(objects[i].key == key) {
			objects[i].object = v;
			return this;
		}
	}

	objects.push_back(ListObject(key, v));

	return this;
}

BdfObject* BdfNamedList::remove(std::string key)
{
	for(int i=0;i<objects.size();i++) {
		if(objects[i].key == key)
		{
			BdfObject* v = objects[i].object;
			objects.erase(objects.begin() + i);
			return v;
		}
	}

	return new BdfObject();
}

BdfObject* BdfNamedList::get(std::string key)
{
	for(int i=0;i<objects.size();i++) {
		if(objects[i].key == key) {
			return objects[i].object;
		}
	}

	BdfObject* v = new BdfObject();
	set(key, v);

	return v;
}

int BdfNamedList::serialize(char **pData)
{
	std::vector<char> data;

	for(ListObject object : objects)
	{
		if(object.object->getType() == BdfTypes::EMPTY) {
			continue;
		}

		const char *object_key = object.key.c_str();
		int32_t size_key = object.key.length();
		char *object_size_key = reverseIfLittleEndian((char*)&size_key, sizeof(size_key));

		for(int i=0;i<sizeof(int32_t);i++) {
			data.push_back(object_size_key[i]);
		}

		for(int i=0;i<size_key;i++) {
			data.push_back(object_key[i]);
		}

		char *object_data;
		int32_t size_data = object.object->serialize(&object_data);
		char *object_size_data = reverseIfLittleEndian((char*)&size_data, sizeof(size_data));

		for(int i=0;i<sizeof(int32_t);i++) {
			data.push_back(object_size_data[i]);
		}

		for(int i=0;i<size_data;i++) {
			data.push_back(object_data[i]);
		}

		free(object_size_key);
		free(object_data);
		free(object_size_data);
	}

	char *data2 = new char[data.size()];

	for(int i=0;i<data.size();i++) {
		data2[i] = data[i];
	}

	*pData = data2;
	return data.size();
}

std::string BdfNamedList::serializeHumanReadable(BdfIndent indent, int it)
{
	if(objects.size() == 0) {
		return "{}";
	}

	std::string data = "{";

	for(int i=0;i<objects.size();i++)
	{
		ListObject list_o = objects[i];

		data += indent.breaker;

		for(int n=0;n<=it;n++) {
			data += indent.indent;
		}

		data += serializeString(list_o.key);
		data += ": ";
		data += list_o.object->serializeHumanReadable(indent, it + 1);

		if(objects.size() > i + 1) {
			data += ", ";
		}
	}

	data += indent.breaker;

	for(int n=0;n<it;n++) {
		data += indent.indent;
	}

	return data + "}";
}
