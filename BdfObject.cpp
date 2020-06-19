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
#include <sstream>

BdfObject::BdfObject(char *pData, int pSize)
{
	if(pSize > 1)
	{
		s = pSize - 1;
		type = pData[0];

		if(type == BdfTypes::STRING) {
			object = new std::string(pData + 1, s);
			return;
		}

		if(type == BdfTypes::ARRAY) {
			object = new BdfArray(pData + 1, s);
			return;
		}

		if(type == BdfTypes::NAMED_LIST) {
			object = new BdfNamedList(pData + 1, s);
			return;
		}

		data = new char[s];
		memcpy(data, pData + 1, s);
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
	delete[] data;
}

char BdfObject::getType() {
	return type;
}

int BdfObject::_serializeSeek()
{
	if(type == BdfTypes::STRING) {
		return ((std::string*)object)->size() + 1;
	}

	if(type == BdfTypes::ARRAY) {
		return ((BdfArray*)object)->_serializeSeek() + 1;
	}

	if(type == BdfTypes::NAMED_LIST) {
		return ((BdfNamedList*)object)->_serializeSeek() + 1;
	}

	return s + 1;
}

int BdfObject::serialize(char **pData)
{
	int size = _serializeSeek();
	*pData = new char[size];

	_serialize(*pData);

	return size;
}

int BdfObject::_serialize(char *pData)
{
	pData[0] = type;

	if(type == BdfTypes::STRING) {
		std::string* str = (std::string*)object;
		const char* bytes = str->c_str();
		memcpy(pData + 1, bytes, str->size());
		return str->size() + 1;
	}

	if(type == BdfTypes::ARRAY) {
		BdfArray* v = (BdfArray*)object;
		int size = v->_serialize(pData + 1);
		return size + 1;
	}

	if(type == BdfTypes::NAMED_LIST) {
		BdfNamedList* v = (BdfNamedList*)object;
		int size = v->_serialize(pData + 1);
		return size + 1;
	}

	memcpy(pData + 1, data, s);
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

void BdfObject::freeAll()
{
	if(type == BdfTypes::ARRAY) {
		((BdfArray*)object)->freeAll();
		delete (BdfArray*)object;
	}

	if(type == BdfTypes::NAMED_LIST) {
		((BdfNamedList*)object)->freeAll();
		delete (BdfNamedList*)object;
	}

	if(type == BdfTypes::STRING) {
		delete (std::string*)object;
	}

	type = BdfTypes::EMPTY;

	delete[] data;
}

void BdfObject::serializeHumanReadable(std::ostream &out) {
	serializeHumanReadable(out, BdfIndent("", ""), 0);
}

void BdfObject::serializeHumanReadable(std::ostream &out, BdfIndent indent) {
	serializeHumanReadable(out, indent, 0);
}

void BdfObject::serializeHumanReadable(std::ostream &out, BdfIndent indent, int it)
{
	switch (type)
	{
		// Objects

		case BdfTypes::ARRAY: {
			((BdfArray*)object)->serializeHumanReadable(out, indent, it);
			return;
		}

		case BdfTypes::NAMED_LIST: {
			((BdfNamedList*)object)->serializeHumanReadable(out, indent, it);
			return;
		}

		case BdfTypes::STRING: {
			out << serializeString(*(std::string*)object);
			return;
		}

		// Primitives

		case BdfTypes::INTEGER: {
			out << std::to_string(getInteger()) << "I";
			return;
		}

		case BdfTypes::BOOLEAN: {
			if(getBoolean()) out << "true";
			else out << "false";
			return;
		}

		case BdfTypes::LONG: {
			out << std::to_string(getLong()) << "L";
			return;
		}

		case BdfTypes::SHORT: {
			out << std::to_string(getShort()) << "S";
			return;
		}

		case BdfTypes::BYTE: {
			out << std::to_string(getByte()) << "B";
			return;
		}

		case BdfTypes::DOUBLE: {
			out << std::to_string(getDouble()) << "D";
			return;
		}

		case BdfTypes::FLOAT: {
			out << std::to_string(getFloat()) << "F";
			return;
		}

		// Arrays

		case BdfTypes::ARRAY_INTEGER:
		{
			out << "(" << calcIndent(indent, it);

			int32_t* v;
			int size = getIntegerArray(&v);

			for(int i=0;i<size;i++) {
				out << indent.breaker << calcIndent(indent, it) << std::to_string(v[i]) << "I";
				if(i != size - 1) out << ", ";
			}

			out << indent.breaker << calcIndent(indent, it - 1) << ")";

			delete[] v;
			return;
		}

		case BdfTypes::ARRAY_BOOLEAN:
		{
			out << "(" << calcIndent(indent, it);

			bool* v;
			int size = getBooleanArray(&v);

			for(int i=0;i<size;i++) {
				out << indent.breaker << calcIndent(indent, it) << (v[i] ? "true" : "false");
				if(i != size - 1) out << ", ";
			}

			out << indent.breaker << calcIndent(indent, it - 1) + ")";

			delete[] v;
			return;
		}

		case BdfTypes::ARRAY_LONG:
		{
			out << "(" << calcIndent(indent, it);

			int64_t* v;
			int size = getLongArray(&v);

			for(int i=0;i<size;i++) {
				out << indent.breaker << calcIndent(indent, it) << std::to_string(v[i]) << "L";
				if(i != size - 1) out << ", ";
			}

			out << indent.breaker << calcIndent(indent, it - 1) << ")";

			delete[] v;
			return;
		}

		case BdfTypes::ARRAY_SHORT:
		{
			out << "(" << calcIndent(indent, it);

			int16_t* v;
			int size = getShortArray(&v);

			for(int i=0;i<size;i++) {
				out << indent.breaker << calcIndent(indent, it) << std::to_string(v[i]) << "S";
				if(i != size - 1) out << ", ";
			}

			out << indent.breaker << calcIndent(indent, it - 1) << ")";

			delete[] v;
			return;
		}

		case BdfTypes::ARRAY_BYTE:
		{
			out << "(" << calcIndent(indent, it);

			char* v;
			int size = getByteArray(&v);

			for(int i=0;i<size;i++) {
				out << indent.breaker << calcIndent(indent, it) << std::to_string((int)v[i]) << "B";
				if(i != size - 1) out << ", ";
			}

			out << indent.breaker << calcIndent(indent, it - 1) << ")";

			delete[] v;
			return;
		}

		case BdfTypes::ARRAY_DOUBLE:
		{
			out << "(" << calcIndent(indent, it);

			double* v;
			int size = getDoubleArray(&v);

			for(int i=0;i<size;i++) {
				out << indent.breaker << calcIndent(indent, it) << std::to_string(v[i]) << "D";
				if(i != size - 1) out << ", ";
			}

			out << indent.breaker << calcIndent(indent, it - 1) << ")";

			delete[] v;
			return;
		}

		case BdfTypes::ARRAY_FLOAT:
		{
			out << "(" << calcIndent(indent, it);

			float* v;
			int size = getFloatArray(&v);

			for(int i=0;i<size;i++) {
				out << indent.breaker << calcIndent(indent, it) << std::to_string(v[i]) << "F";
				if(i != size - 1) out << ", ";
			}

			out << indent.breaker << calcIndent(indent, it - 1) << ")";

			delete[] v;
			return;
		}

		default: {
			out << "undefined";
			return;
		}
	}
}

std::string BdfObject::serializeHumanReadable(BdfIndent indent)
{
	std::stringstream out;

	serializeHumanReadable(out, indent, 0);

	return out.str();
}

std::string BdfObject::serializeHumanReadable() {
	return serializeHumanReadable(BdfIndent("", ""));
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
		free(object);
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
	delete[] data;

	s = sizeof(v);
	type = BdfTypes::INTEGER;
	data = reverseIfLittleEndian(&v, sizeof(v));
	return this;
}

BdfObject* BdfObject::setBoolean(bool v)
{
	delete[] data;

	s = 1;
	data = new char[1] {(char)(v ? 0x01 : 0x00)};
	type = BdfTypes::BOOLEAN;
	return this;
}

BdfObject* BdfObject::setLong(int64_t v)
{
	delete[] data;

	s = sizeof(v);
	type = BdfTypes::LONG;
	data = reverseIfLittleEndian(&v, sizeof(v));
	return this;
}

BdfObject* BdfObject::setShort(int16_t v)
{
	delete[] data;

	s = sizeof(v);
	type = BdfTypes::SHORT;
	data = reverseIfLittleEndian(&v, sizeof(v));
	return this;
}

BdfObject* BdfObject::setByte(char v)
{
	delete[] data;

	s = sizeof(v);
	type = BdfTypes::BYTE;
	data = reverseIfLittleEndian(&v, sizeof(v));
	return this;
}

BdfObject* BdfObject::setDouble(double v)
{
	delete[] data;

	s = sizeof(v);
	type = BdfTypes::DOUBLE;
	data = reverseIfLittleEndian(&v, sizeof(v));
	return this;
}

BdfObject* BdfObject::setFloat(float v)
{
	delete[] data;

	s = sizeof(v);
	type = BdfTypes::FLOAT;
	data = reverseIfLittleEndian(&v, sizeof(v));
	return this;
}

// Arrays

BdfObject* BdfObject::setIntegerArray(int32_t* v, int size)
{
	int l = sizeof(int32_t);
	delete[] data;

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
	delete[] data;

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
	delete[] data;

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
	delete[] data;

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
	delete[] data;

	s = size;
	type = BdfTypes::ARRAY_BYTE;

	data = new char[s];
	memcpy(data, v, size);

	return this;
}

BdfObject* BdfObject::setDoubleArray(double* v, int size)
{
	int l = sizeof(double);
	delete[] data;

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
	delete[] data;

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
	delete[] data;
	type = BdfTypes::STRING;
	object = &v;

	return this;
}

BdfObject* BdfObject::setArray(BdfArray *v)
{
	delete[] data;
	type = BdfTypes::ARRAY;
	object = v;

	return this;
}

BdfObject* BdfObject::setNamedList(BdfNamedList *v)
{
	delete[] data;
	type = BdfTypes::NAMED_LIST;
	object = v;

	return this;
}
