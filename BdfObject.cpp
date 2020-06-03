/*
 * bdfobject.cpp
 *
 *  Created on: 30 May 2020
 *      Author: josua
 */

#include "headers.h"
#include "helpers.h"
#include <iostream>
#include <string.h>

BdfObject::BdfObject(char *pData, int pSize)
{
	if(pSize > 1)
	{
		s = pSize - 1;
		type = pData[0];
		data = new char[s];
		memcpy(data, pData + 1, s);

		if(type == BdfTypes::STRING) {
			object = new std::string(data, s);
		}

		if(type == BdfTypes::ARRAY) {
			object = new BdfArray(data, s);
		}

		if(type == BdfTypes::NAMED_LIST) {
			object = new BdfNamedList(data, s);
		}
	}

	else {
		s = 0;
		type = BdfTypes::EMPTY;
		data = new char[0];
	}
}

BdfObject::BdfObject() : BdfObject(new char[0], 0) {

}

BdfObject::~BdfObject() {
	//delete[] data;
}

char BdfObject::getType() {
	return type;
}

int BdfObject::serialize(char **pData)
{
	if(type == BdfTypes::STRING) {
		std::string* str = (std::string*)object;
		const char* bytes = str->c_str();
		data = new char[str->size()];
		memcpy(data, bytes, str->size());
		s = str->size();
	}

	if(type == BdfTypes::ARRAY) {
		BdfArray* v = (BdfArray*)object;
		s = v->serialize(&data);
	}

	if(type == BdfTypes::NAMED_LIST) {
		BdfNamedList* v = (BdfNamedList*)object;
		s = v->serialize(&data);
	}

	*pData = new char[s + 1];
	memcpy(*pData + 1, data, s);
	*pData[0] = type;

	return s + 1;
}

std::string calcIndent(BdfIndent indent, int it)
{
	std::string t = "";

	for(int i=0;i<=it;i++) {
		t += indent.indent;
	}

	return t;
}

std::string BdfObject::serializeHumanReadable(BdfIndent indent, int it)
{
	if(type == BdfTypes::BOOLEAN) {
		if(getBoolean()) return "true";
		else return "false";
	}

	// Objects
	if(type == BdfTypes::ARRAY) return ((BdfArray*)object)->serializeHumanReadable(indent, it);
	if(type == BdfTypes::NAMED_LIST) return ((BdfNamedList*)object)->serializeHumanReadable(indent, it);
	if(type == BdfTypes::STRING) return serializeString(*(std::string*)object);

	// Primitives
	if(type == BdfTypes::INTEGER) return std::to_string(getInteger())+"I";
	if(type == BdfTypes::SHORT) return std::to_string(getShort())+"S";
	if(type == BdfTypes::LONG) return std::to_string(getLong())+"L";
	if(type == BdfTypes::BYTE) return std::to_string((int)getByte())+"B";
	if(type == BdfTypes::DOUBLE) return std::to_string(getDouble())+"D";
	if(type == BdfTypes::FLOAT) return std::to_string(getFloat())+"F";

	// Arrays

	if(type == BdfTypes::ARRAY_INTEGER)
	{
		std::string str = "(" + calcIndent(indent, it);

		int32_t* v;
		int size = getIntegerArray(&v);

		for(int i=0;i<size;i++) {
			str += indent.breaker + calcIndent(indent, it) + std::to_string(v[i]) + "I, ";
		}

		str = str.substr(0, str.size() - 2);
		str += indent.breaker + calcIndent(indent, it - 1) + ")";

		return str;
	}

	if(type == BdfTypes::ARRAY_BOOLEAN)
	{
		std::string str = "(" + calcIndent(indent, it);

		bool* v;
		int size = getBooleanArray(&v);

		for(int i=0;i<size;i++) {
			str += indent.breaker + calcIndent(indent, it) + (v[i] ? "true" : "false") + ", ";
		}

		str = str.substr(0, str.size() - 2);
		str += indent.breaker + calcIndent(indent, it - 1) + ")";

		return str;
	}

	if(type == BdfTypes::ARRAY_SHORT)
	{
		std::string str = "(" + calcIndent(indent, it);

		int16_t* v;
		int size = getShortArray(&v);

		for(int i=0;i<size;i++) {
			str += indent.breaker + calcIndent(indent, it) + std::to_string(v[i]) + "S, ";
		}

		str = str.substr(0, str.size() - 2);
		str += indent.breaker + calcIndent(indent, it - 1) + ")";

		return str;
	}

	if(type == BdfTypes::ARRAY_LONG)
	{
		std::string str = "(" + calcIndent(indent, it);

		int64_t* v;
		int size = getLongArray(&v);

		for(int i=0;i<size;i++) {
			str += indent.breaker + calcIndent(indent, it) + std::to_string(v[i]) + "L, ";
		}

		str = str.substr(0, str.size() - 2);
		str += indent.breaker + calcIndent(indent, it - 1) + ")";

		return str;
	}

	if(type == BdfTypes::ARRAY_BYTE)
	{
		std::string str = "(" + calcIndent(indent, it);

		char* v;
		int size = getByteArray(&v);

		for(int i=0;i<size;i++) {
			str += indent.breaker + calcIndent(indent, it) + std::to_string((int)v[i]) + "B, ";
		}

		str = str.substr(0, str.size() - 2);
		str += indent.breaker + calcIndent(indent, it - 1) + ")";

		return str;
	}

	if(type == BdfTypes::ARRAY_DOUBLE)
	{
		std::string str = "(" + calcIndent(indent, it);

		double* v;
		int size = getDoubleArray(&v);

		for(int i=0;i<size;i++) {
			str += indent.breaker + calcIndent(indent, it) + std::to_string(v[i]) + "D, ";
		}

		str = str.substr(0, str.size() - 2);
		str += indent.breaker + calcIndent(indent, it - 1) + ")";

		return str;
	}

	if(type == BdfTypes::ARRAY_INTEGER)
	{
		std::string str = "(" + calcIndent(indent, it);

		float* v;
		int size = getFloatArray(&v);

		for(int i=0;i<size;i++) {
			str += indent.breaker + calcIndent(indent, it) + std::to_string(v[i]) + "F, ";
		}

		str = str.substr(0, str.size() - 2);
		str += indent.breaker + calcIndent(indent, it - 1) + ")";

		return str;
	}

	return "undefined";
}

std::string BdfObject::serializeHumanReadable(BdfIndent indent) {
	return serializeHumanReadable(indent, 0);
}

std::string BdfObject::serializeHumanReadable() {
	return serializeHumanReadable(BdfIndent("", ""), 0);
}

// Get

// Primitives

int32_t BdfObject::getInteger()
{
	if(type != BdfTypes::INTEGER) {
		return 0;
	}

	char* data2 = reverseIfLittleEndian(data, s);
	uint32_t v = *(uint32_t*)data2;
	delete[] data2;
	return v;
}

bool BdfObject::getBoolean()
{
	if(type == BdfTypes::BOOLEAN) {
		return data[0] == 0x01;
	} else {
		return false;
	}
}

int64_t BdfObject::getLong()
{
	if(type != BdfTypes::LONG) {
		return 0;
	}

	char* data2 = reverseIfLittleEndian(data, s);
	uint64_t v = *(uint64_t*)data2;
	delete[] data2;
	return v;
}

int16_t BdfObject::getShort()
{
	if(type != BdfTypes::SHORT) {
		return 0;
	}

	char* data2 = reverseIfLittleEndian(data, s);
	uint16_t v = *(uint16_t*)data2;
	delete[] data2;
	return v;
}

char BdfObject::getByte()
{
	if(type != BdfTypes::BYTE) {
		return 0;
	}

	return data[0];
}

double BdfObject::getDouble()
{
	if(type != BdfTypes::DOUBLE) {
		return 0;
	}

	char* data2 = reverseIfLittleEndian(data, s);
	double v = *(double*)data2;
	delete[] data2;
	return v;
}

float BdfObject::getFloat()
{
	if(type != BdfTypes::FLOAT) {
		return 0;
	}

	char* data2 = reverseIfLittleEndian(data, s);
	float v = *(float*)data2;
	delete[] data2;
	return v;
}

// Arrays

int BdfObject::getIntegerArray(int32_t** v)
{
	if(type != BdfTypes::ARRAY_INTEGER) {
		*v = new int32_t[0];
		return 0;
	}

	int l = sizeof(int32_t);
	int size = s / l;

	*v = new int32_t[s];
	char* bytes = (char*)(*v);

	for(int i=0;i<size;i++)
	{
		char* bytes2 = new char[l];
		memcpy(bytes2, data + l*i, l);
		bytes2 = reverseIfLittleEndian(bytes2, l);
		memcpy(bytes + l*i, bytes2, l);
		delete[] bytes2;
	}

	return size;
}

int BdfObject::getBooleanArray(bool** v)
{
	if(type != BdfTypes::ARRAY_BOOLEAN) {
		*v = new bool[0];
		return 0;
	}

	*v = new bool[s];
	bool* bytes = *v;

	for(int i=0;i<s;i++) {
		bytes[i] = (data[i] == 0x01 ? true : false);
	}

	return s;
}

int BdfObject::getShortArray(int16_t** v)
{
	if(type != BdfTypes::ARRAY_SHORT) {
		*v = new int16_t[0];
		return 0;
	}

	int l = sizeof(int16_t);
	int size = s / l;

	*v = new int16_t[s];
	char* bytes = (char*)(*v);

	for(int i=0;i<size;i++)
	{
		char* bytes2 = new char[l];
		memcpy(bytes2, data + l*i, l);
		bytes2 = reverseIfLittleEndian(bytes2, l);
		memcpy(bytes + l*i, bytes2, l);
		delete[] bytes2;
	}

	return size;
}

int BdfObject::getLongArray(int64_t** v)
{
	if(type != BdfTypes::ARRAY_LONG) {
		*v = new int64_t[0];
		return 0;
	}

	int l = sizeof(int64_t);
	int size = s / l;

	*v = new int64_t[s];
	char* bytes = (char*)(*v);

	for(int i=0;i<size;i++)
	{
		char* bytes2 = new char[l];
		memcpy(bytes2, data + l*i, l);
		bytes2 = reverseIfLittleEndian(bytes2, l);
		memcpy(bytes + l*i, bytes2, l);
		delete[] bytes2;
	}

	return size;
}

int BdfObject::getByteArray(char** v)
{
	if(type != BdfTypes::ARRAY_BYTE) {
		*v = new char[0];
		return 0;
	}

	*v = new char[s];
	memcpy(*v, data, s);

	return s;
}

int BdfObject::getDoubleArray(double** v)
{
	if(type != BdfTypes::ARRAY_DOUBLE) {
		*v = new double[0];
		return 0;
	}

	int l = sizeof(double);
	int size = s / l;

	*v = new double[s];
	char* bytes = (char*)(*v);

	for(int i=0;i<size;i++)
	{
		char* bytes2 = new char[l];
		memcpy(bytes2, data + l*i, l);
		bytes2 = reverseIfLittleEndian(bytes2, l);
		memcpy(bytes + l*i, bytes2, l);
		delete[] bytes2;
	}

	return size;
}

int BdfObject::getFloatArray(float** v)
{
	if(type != BdfTypes::ARRAY_FLOAT) {
		*v = new float[0];
		return 0;
	}

	int l = sizeof(float);
	int size = s / l;

	*v = new float[s];
	char* bytes = (char*)(*v);

	for(int i=0;i<size;i++)
	{
		char* bytes2 = new char[l];
		memcpy(bytes2, data + l*i, l);
		bytes2 = reverseIfLittleEndian(bytes2, l);
		memcpy(bytes + l*i, bytes2, l);
		delete[] bytes2;
	}

	return size;
}

// Objects

std::string BdfObject::getString()
{
	std::string* v;

	if(type == BdfTypes::STRING) {
		v = (std::string*)object;
	} else {
		*v = "";
	}

	type = BdfTypes::STRING;
	object = v;
	return *v;
}

BdfArray* BdfObject::getArray()
{
	BdfArray* v;

	if(type == BdfTypes::ARRAY) {
		v = (BdfArray*)object;
	} else {
		v = new BdfArray();
	}

	type = BdfTypes::ARRAY;
	object = v;
	return v;
}

BdfNamedList* BdfObject::getNamedList()
{
	BdfNamedList* v;

	if(type == BdfTypes::NAMED_LIST) {
		v = (BdfNamedList*)object;
	} else {
		v = new BdfNamedList();
	}

	type = BdfTypes::NAMED_LIST;
	object = v;
	return v;
}

// Set

// Primitives

BdfObject* BdfObject::setInteger(int32_t v)
{
	s = sizeof(v);
	type = BdfTypes::INTEGER;
	data = reverseIfLittleEndian(&v, sizeof(v));
	return this;
}

BdfObject* BdfObject::setBoolean(bool v)
{
	s = 1;
	data = new char[1] {(char)(v ? 0x01 : 0x00)};
	type = BdfTypes::BOOLEAN;
	return this;
}

BdfObject* BdfObject::setLong(int64_t v)
{
	s = sizeof(v);
	type = BdfTypes::LONG;
	data = reverseIfLittleEndian(&v, sizeof(v));
	return this;
}

BdfObject* BdfObject::setShort(int16_t v)
{
	s = sizeof(v);
	type = BdfTypes::SHORT;
	data = reverseIfLittleEndian(&v, sizeof(v));
	return this;
}

BdfObject* BdfObject::setByte(char v)
{
	s = sizeof(v);
	type = BdfTypes::BYTE;
	data = reverseIfLittleEndian(&v, sizeof(v));
	return this;
}

BdfObject* BdfObject::setDouble(double v)
{
	s = sizeof(v);
	type = BdfTypes::DOUBLE;
	data = reverseIfLittleEndian(&v, sizeof(v));
	return this;
}

BdfObject* BdfObject::setFloat(float v)
{
	s = sizeof(v);
	type = BdfTypes::FLOAT;
	data = reverseIfLittleEndian(&v, sizeof(v));
	return this;
}

// Arrays

BdfObject* BdfObject::setIntegerArray(int32_t* v, int size)
{
	int l = sizeof(int32_t);

	s = l * size;
	data = new char[s];
	type = BdfTypes::ARRAY_INTEGER;

	for(int i=0;i<size;i++)
	{
		char* bytes = new char[l];
		bytes = reverseIfLittleEndian(&v[i], l);
		memcpy(data + i*l, bytes, l);
		delete[] bytes;
	}

	return this;
}

BdfObject* BdfObject::setBooleanArray(bool* v, int size)
{
	s = size;
	data = new char[s];
	type = BdfTypes::ARRAY_BOOLEAN;

	for(int i=0;i<s;i++) {
		data[i] = (char)(v[i] ? 0x01 : 0x00);
	}

	return this;
}

BdfObject* BdfObject::setShortArray(int16_t* v, int size)
{
	int l = sizeof(int16_t);

	s = l * size;
	data = new char[s];
	type = BdfTypes::ARRAY_SHORT;

	for(int i=0;i<size;i++)
	{
		char* bytes = new char[l];
		bytes = reverseIfLittleEndian(&v[i], l);
		memcpy(data + i*l, bytes, l);
		delete[] bytes;
	}

	return this;
}

BdfObject* BdfObject::setLongArray(int64_t* v, int size)
{
	int l = sizeof(int64_t);

	s = l * size;
	data = new char[s];
	type = BdfTypes::ARRAY_LONG;

	for(int i=0;i<size;i++)
	{
		char* bytes = new char[l];
		bytes = reverseIfLittleEndian(&v[i], l);
		memcpy(data + i*l, bytes, l);
		delete[] bytes;
	}

	return this;
}

BdfObject* BdfObject::setByteArray(char* v, int size)
{
	s = size;
	type = BdfTypes::ARRAY_BYTE;

	data = new char[s];
	memcpy(data, v, size);

	return this;
}

BdfObject* BdfObject::setDoubleArray(double* v, int size)
{
	int l = sizeof(double);

	s = l * size;
	data = new char[s];
	type = BdfTypes::ARRAY_DOUBLE;

	for(int i=0;i<size;i++)
	{
		char* bytes = new char[l];
		bytes = reverseIfLittleEndian(&v[i], l);
		memcpy(data + i*l, bytes, l);
		delete[] bytes;
	}

	return this;
}

BdfObject* BdfObject::setFloatArray(float* v, int size)
{
	int l = sizeof(float);

	s = l * size;
	data = new char[s];
	type = BdfTypes::ARRAY_FLOAT;

	for(int i=0;i<size;i++)
	{
		char* bytes = new char[l];
		bytes = reverseIfLittleEndian(&v[i], l);
		memcpy(data + i*l, bytes, l);
		delete[] bytes;
	}

	return this;
}

// Objects

BdfObject* BdfObject::setString(std::string v)
{
	type = BdfTypes::STRING;
	object = &v;

	return this;
}

BdfObject* BdfObject::setArray(BdfArray *v)
{
	type = BdfTypes::ARRAY;
	object = v;

	return this;
}

BdfObject* BdfObject::setNamedList(BdfNamedList *v)
{
	type = BdfTypes::NAMED_LIST;
	object = v;

	return this;
}
