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
		char* bytes_data = pData + i;

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

int BdfNamedList::_serializeSeek()
{
	int size = 0;

	for(ListObject object : objects)
	{
		if(object.object->getType() == BdfTypes::EMPTY) {
			continue;
		}

		size += object.key.size();
		size += object.object->_serializeSeek();
		size += 8;
	}

	return size;
}

int BdfNamedList::_serialize(char* data)
{
	int size = 0;

	for(ListObject object : objects)
	{
		// Get the size of the key
		int32_t size_key = object.key.size();
		char* size_key_c = reverseIfLittleEndian(&size_key, 4);

		// Copy the key size
		memcpy(data + size, size_key_c, 4);
		delete[] size_key_c;
		size += 4;

		// Send back the key size
		const char* bytes_key = object.key.c_str();

		// Send back the key
		memcpy(data + size, bytes_key, size_key);
		size += size_key;

		// Get the object
		int32_t size_object = object.object->_serialize(data + size + 4);
		char* size_object_c = reverseIfLittleEndian(&size_object, 4);

		// Send back the object
		memcpy(data + size, size_object_c, 4);
		delete[] size_object_c;
		size += size_object;
		size += 4;
	}

	return size;
}

void BdfNamedList::serializeHumanReadable(std::ostream &out, BdfIndent indent, int it)
{
	if(objects.size() == 0) {
		out << "{}";
		return;
	}

	out << "{";

	for(int i=0;i<objects.size();i++)
	{
		ListObject list_o = objects[i];

		out << indent.breaker;

		for(int n=0;n<=it;n++) {
			out << indent.indent;
		}

		out << serializeString(list_o.key) << ": ";
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

void BdfNamedList::freeAll()
{
	for(ListObject o : objects) {
		o.object->freeAll();
	}
}
