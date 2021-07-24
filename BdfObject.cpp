
#include "Bdf.hpp"
#include "BdfHelpers.hpp"
#include <iostream>
#include <string.h>
#include <sstream>
#include <math.h>

using namespace Bdf;
using namespace BdfHelpers;

bool shouldStoreSize(char b) {
	return b > 7;
}

char BdfObject::getSizeBytes(char size_bytes_tag)
{
	switch(size_bytes_tag)
	{
		case 0: return 4;
		case 1: return 2;
		case 2: return 1;
		default: return 4;
	}
}

int getDefaultSize(char type)
{
	switch(type)
	{
		case BdfTypes::BOOLEAN:
			return 2;
		case BdfTypes::BYTE:
			return 2;
		case BdfTypes::DOUBLE:
			return 9;
		case BdfTypes::FLOAT:
			return 5;
		case BdfTypes::INTEGER:
			return 5;
		case BdfTypes::LONG:
			return 9;
		case BdfTypes::SHORT:
			return 3;
		case BdfTypes::UNDEFINED:
			return 1;
		default:
			return -1;
	}
}

int BdfObject::getSize(const char* data)
{
	char type, size_tag;
	getFlagData(data, &type, &size_tag, NULL);

	int size = getDefaultSize(type);

	if(size != -1) {
		return size;
	}

	char size_bytes = getSizeBytes(size_tag);

	switch(size_bytes)
	{
		case 4: return get_netsi(data + 1);
		case 2: return get_netus(data + 1);
		case 1: return data[1] & 255;
	}

	return 0;
}

void BdfObject::getFlagData(const char* data, char* pType, char* pSizeBytes, char* pParentFlags)
{
	unsigned char flags = *(unsigned char*)data;
	
	unsigned char type = flags % 18;
	flags = (flags - type) / 18;

	unsigned char size_bytes = flags % 3;
	flags = (flags - size_bytes) / 3;

	unsigned char parent_flags = flags % 3;
	flags = (flags - parent_flags) / 3;

	if(pType != NULL)
		*pType = type;

	if(pSizeBytes != NULL)
		*pSizeBytes = size_bytes;
	
	if(pParentFlags != NULL)
		*pParentFlags = parent_flags;
}

BdfObject::BdfObject(BdfLookupTable* pLookupTable, const char *pData, int pSize)
{
	s = 0;
	last_seek = 0;
	data = NULL;
	object = NULL;
	type = BdfTypes::UNDEFINED;
	lookupTable = pLookupTable;

	if(pSize > 1)
	{
		// Get the type and database values
		unsigned char flags = *(unsigned char*)pData;
		type = (char)(flags % 18);
		flags = (char)((flags - type) / 18);
		char size_bytes = getSizeBytes(flags % 3);
		
		const char* oData = pData + 1;
		s = pSize - 1;

		if(shouldStoreSize(type)) {
			oData += size_bytes;
			s -= size_bytes;
		}

		if(s < 0) {
			type = BdfTypes::UNDEFINED;
			s = 0;
			return;
		}

		switch(type)
		{
			case BdfTypes::STRING:
				object = new std::string(oData, s);
				break;
			case BdfTypes::LIST:
				object = new BdfList(lookupTable, oData, s);
				break;
			case BdfTypes::NAMED_LIST:
				object = new BdfNamedList(lookupTable, oData, s);
				break;
			case BdfTypes::UNDEFINED:
				return;
		}

		if(object == NULL) {
			data = new char[s];
			memcpy(data, oData, s);
		}
	}

	else {
		s = 0;
		type = BdfTypes::UNDEFINED;
	}
}

BdfObject::BdfObject(BdfLookupTable* lookupTable) : BdfObject(lookupTable, NULL, 0) {

}

void freeTypedArray(void* array, char type)
{
	switch(type)
	{
		case BdfTypes::ARRAY_INTEGER:
			delete[] (int32_t*)array;
			return;
		case BdfTypes::ARRAY_SHORT:
			delete[] (int16_t*)array;
			return;
		case BdfTypes::ARRAY_LONG:
			delete[] (int64_t*)array;
			return;
		case BdfTypes::ARRAY_BYTE:
			delete[] (char*)array;
			return;
		case BdfTypes::ARRAY_BOOLEAN:
			delete[] (bool*)array;
			return;
		case BdfTypes::ARRAY_DOUBLE:
			delete[] (double*)array;
			return;
		case BdfTypes::ARRAY_FLOAT:
			delete[] (float*)array;
			return;
	}
}

BdfObject::BdfObject(BdfLookupTable* pLookupTable, BdfStringReader* sr)
{
	s = 0;
	last_seek = 0;
	data = NULL;
	object = NULL;
	type = BdfTypes::UNDEFINED;
	lookupTable = pLookupTable;

	wchar_t c = sr->upto[0];
	
	if(c == '{') {
		setNamedList(new BdfNamedList(lookupTable, sr));
		return;
	}

	if(c == '[') {
		setList(new BdfList(lookupTable, sr));
		return;
	}

	if(c == '"') {
		setString(sr->getQuotedString());
		return;
	}

	bool isDecimalArray = false;
	bool isPrimitiveArray = false;
	char type = 0;

	if(sr->isNext(L"int")) {
		type = BdfTypes::ARRAY_INTEGER;
		isPrimitiveArray = true;
	}

	else if(sr->isNext(L"long")) {
		type = BdfTypes::ARRAY_LONG;
		isPrimitiveArray = true;
	}

	else if(sr->isNext(L"byte")) {
		type = BdfTypes::ARRAY_BYTE;
		isPrimitiveArray = true;
	}

	else if(sr->isNext(L"short")) {
		type = BdfTypes::ARRAY_SHORT;
		isPrimitiveArray = true;
	}

	else if(sr->isNext(L"bool")) {
		type = BdfTypes::ARRAY_BOOLEAN;
		isPrimitiveArray = true;
	}

	else if(sr->isNext(L"double")) {
		type = BdfTypes::ARRAY_DOUBLE;
		isPrimitiveArray = true;
		isDecimalArray = true;
	}

	else if(sr->isNext(L"float")) {
		type = BdfTypes::ARRAY_FLOAT;
		isPrimitiveArray = true;
		isDecimalArray = true;
	}

	// Deserialize a primitive array
	if(isPrimitiveArray)
	{
		sr->ignoreBlanks();

		if(sr->upto[0] != '(') {
			throw BdfError(BdfError::ERROR_SYNTAX, *sr);
		}

		sr->upto += 1;
		sr->ignoreBlanks();

		// Get the size of the primitive array
		int size = 0;
		BdfStringReader sr2 = *sr;	
		
		for(;;)
		{
			if(sr2.upto[0] == ')') {
				sr2.upto += 1;
				break;
			}

			if(
					sr2.isNext(L"true") || sr2.isNext(L"false") ||
					sr2.isNext(L"infinityf") || sr2.isNext(L"-infinityf") ||
					sr2.isNext(L"infinityd") || sr2.isNext(L"-infinityd") ||
					sr2.isNext(L"nanf") || sr2.isNext(L"nand") )
			{
				size += 1;
			}

			else
			{
				for(;;)
				{
					if(!sr2.inRange()) {
						throw BdfError(BdfError::ERROR_END_OF_FILE, sr2.getPointer(-1));
					}
						
					c = sr2.upto[0];

					if(c >= 'a' && c <= 'z') {
						c -= 32;
					}

					if((c >= '0' && c <= '9') || ((c == '.' || c == 'E') && isDecimalArray) || c == '-' || c == '+') {
						sr2.upto += 1;
						continue;
					}

					if(c == 'B' || c == 'S' || c == 'I' || c == 'L' || c == 'D' || c == 'F') {
						sr2.upto += 1;
						size += 1;
						break;
					}

					throw BdfError(BdfError::ERROR_SYNTAX, sr2);
				}
			}

			sr2.ignoreBlanks();

			if(sr2.upto[0] == ',') {
				sr2.upto += 1;
				sr2.ignoreBlanks();
			}

			if(sr2.upto[0] == ')') {
				sr2.upto += 1;
				break;
			}
		}

		void* array = nullptr;

		switch(type)
		{
			case BdfTypes::ARRAY_BOOLEAN:
				array = new bool[size];
				break;
			case BdfTypes::ARRAY_BYTE:
				array = new char[size];
				break;
			case BdfTypes::ARRAY_DOUBLE:
				array = new double[size];
				break;
			case BdfTypes::ARRAY_FLOAT:
				array = new float[size];
				break;
			case BdfTypes::ARRAY_INTEGER:
				array = new int32_t[size];
				break;
			case BdfTypes::ARRAY_LONG:
				array = new int64_t[size];
				break;
			case BdfTypes::ARRAY_SHORT:
				array = new int16_t[size];
				break;
		}

		for(int i=0;;i++)
		{
			if(sr->upto[0] == ')') {
				sr->upto += 1;
				break;
			}

			if(sr->isNext(L"true"))
			{
				if(type != BdfTypes::ARRAY_BOOLEAN) {
					freeTypedArray(array, type);
					throw BdfError(BdfError::ERROR_SYNTAX, sr->getPointer(-4), 4);
				}

				bool* a = (bool*)array;
				a[i] = true;
			}

			else if(sr->isNext(L"false"))
			{
				if(type != BdfTypes::ARRAY_BOOLEAN) {
					freeTypedArray(array, type);
					throw BdfError(BdfError::ERROR_SYNTAX, sr->getPointer(-5), 5);
				}

				bool* a = (bool*)array;
				a[i] = false;
			}

			else if(sr->isNext(L"infinityd"))
			{
				if(type != BdfTypes::ARRAY_DOUBLE) {
					freeTypedArray(array, type);
					throw BdfError(BdfError::ERROR_SYNTAX, sr->getPointer(-9), 9);
				}
				
				double* a = (double*)array;
				a[i] = INFINITY;
			}
			
			else if(sr->isNext(L"-infinityd"))
			{
				if(type != BdfTypes::ARRAY_DOUBLE) {
					freeTypedArray(array, type);
					throw BdfError(BdfError::ERROR_SYNTAX, sr->getPointer(-10), 10);
				}
				
				double* a = (double*)array;
				a[i] = -INFINITY;
			}

			else if(sr->isNext(L"nand"))
			{
				if(type != BdfTypes::ARRAY_DOUBLE) {
					freeTypedArray(array, type);
					throw BdfError(BdfError::ERROR_SYNTAX, sr->getPointer(-4), 4);
				}

				double* a = (double*)array;
				a[i] = NAN;
			}

			else if(sr->isNext(L"infinityf"))
			{
				if(type != BdfTypes::ARRAY_FLOAT) {
					freeTypedArray(array, type);
					throw BdfError(BdfError::ERROR_SYNTAX, sr->getPointer(-9), 9);
				}
				
				float* a = (float*)array;
				a[i] = INFINITY;
			}
			
			else if(sr->isNext(L"-infinityf"))
			{
				if(type != BdfTypes::ARRAY_FLOAT) {
					freeTypedArray(array, type);
					throw BdfError(BdfError::ERROR_SYNTAX, sr->getPointer(-10), 10);
				}
				
				float* a = (float*)array;
				a[i] = -INFINITY;
			}

			else if(sr->isNext(L"nanf"))
			{
				if(type != BdfTypes::ARRAY_FLOAT) {
					freeTypedArray(array, type);
					throw BdfError(BdfError::ERROR_SYNTAX, sr->getPointer(-4), 4);
				}
				
				float* a = (float*)array;
				a[i] = NAN;
			}

			else
			{
				// Parse a number
				std::string number = "";

				for(;;)
				{
					if(!sr->inRange()) {
						freeTypedArray(array, type);
						throw BdfError(BdfError::ERROR_END_OF_FILE, *sr);
					}

					c = sr->upto[0];

					if(c >= 'a' && c <= 'z') {
						c -= 32;
					}

					if((c >= '0' && c <= '9') || ((c == '.' || c == 'E') && isDecimalArray) || c == '-' || c == '+') {
						sr->upto += 1;
						number += c;
						continue;
					}
					
					try
					{
						switch(c)
						{
							case 'D':
							{
								if(type != BdfTypes::ARRAY_DOUBLE) {
									freeTypedArray(array, type);
									throw BdfError(BdfError::ERROR_SYNTAX, *sr);
								}
	
								double* a = (double*) array;
								a[i] = std::stod(number);
	
								sr->upto += 1;
								break;
							}
	
							case 'F':
							{
								if(type != BdfTypes::ARRAY_FLOAT) {
									freeTypedArray(array, type);
									throw BdfError(BdfError::ERROR_SYNTAX, *sr);
								}
	
								float* a = (float*) array;
								a[i] = std::stof(number);
	
								sr->upto += 1;
								break;
							}
	
							case 'I':
							{
								if(type != BdfTypes::ARRAY_INTEGER) {
									freeTypedArray(array, type);
									throw BdfError(BdfError::ERROR_SYNTAX, *sr);
								}
	
								int32_t* a = (int32_t*) array;
								a[i] = (int32_t)std::stol(number);
	
								sr->upto += 1;
								break;
							}
	
							case 'L':
							{
								if(type != BdfTypes::ARRAY_LONG) {
									freeTypedArray(array, type);
									throw BdfError(BdfError::ERROR_SYNTAX, *sr);
								}
	
								int64_t* a = (int64_t*) array;
								a[i] = (int64_t)std::stol(number);
	
								sr->upto += 1;
								break;
							}
	
							case 'S':
							{
								if(type != BdfTypes::ARRAY_SHORT) {
									freeTypedArray(array, type);
									throw BdfError(BdfError::ERROR_SYNTAX, *sr);
								}
	
								int16_t* a = (int16_t*) array;
								a[i] = (int16_t)std::stoi(number);
	
								sr->upto += 1;
								break;
							}
	
							case 'B':
							{
								if(type != BdfTypes::ARRAY_BYTE) {
									freeTypedArray(array, type);
									throw BdfError(BdfError::ERROR_SYNTAX, *sr);
								}
	
								char* a = (char*) array;
								a[i] = (char)std::stoi(number);
	
								sr->upto += 1;
								break;
							}
	
							default:
								freeTypedArray(array, type);
								throw BdfError(BdfError::ERROR_SYNTAX, *sr);	
						}

					}

					catch(std::invalid_argument &e) {
						freeTypedArray(array, type);
						throw BdfError(BdfError::ERROR_SYNTAX, sr->getPointer(-number.size()), number.size() + 1);
					}

					catch(std::out_of_range &e) {
						freeTypedArray(array, type);
						throw BdfError(BdfError::ERROR_OUT_OF_RANGE, sr->getPointer(-number.size()), number.size() + 1);
					}

					break;
				}
			}

			// int (420I, 23I  )
			
			sr->ignoreBlanks();

			if(sr->upto[0] == ',') {
				sr->upto += 1;
				sr->ignoreBlanks();
			}

			if(sr->upto[0] == ')') {
				sr->upto += 1;
				break;
			}
		}

		switch(type)
		{
			case BdfTypes::ARRAY_BOOLEAN:
				setBooleanArray((bool*)array, size);
				break;
			case BdfTypes::ARRAY_BYTE:
				setByteArray((char*)array, size);
				break;
			case BdfTypes::ARRAY_DOUBLE:
				setDoubleArray((double*)array, size);
				break;
			case BdfTypes::ARRAY_FLOAT:
				setFloatArray((float*)array, size);
				break;
			case BdfTypes::ARRAY_INTEGER:
				setIntegerArray((int32_t*)array, size);
				break;
			case BdfTypes::ARRAY_LONG:
				setLongArray((int64_t*)array, size);
				break;
			case BdfTypes::ARRAY_SHORT:
				setShortArray((short*)array, size);
				break;
		}

		freeTypedArray(array, type);

		return;
	}

	if(sr->isNext(L"true")) {
		setBoolean(true);
		return;
	}

	if(sr->isNext(L"false")) {
		setBoolean(false);
		return;
	}

	if(sr->isNext(L"infinityd")) {
		setDouble(INFINITY);
		return;
	}

	if(sr->isNext(L"-infinityd")) {
		setDouble(-INFINITY);
		return;
	}

	if(sr->isNext(L"nand")) {
		setDouble(NAN);
		return;
	}

	if(sr->isNext(L"infinityf")) {
		setFloat(INFINITY);
		return;
	}
	
	if(sr->isNext(L"-infinityf")) {
		setFloat(-INFINITY);
		return;
	}
	
	if(sr->isNext(L"nanf")) {
		setFloat(NAN);
		return;
	}

	if(sr->isNext(L"undefined")) {
		return;
	}

	// Parse a number
	std::string number = "";

	bool isDecimal = false;

	for(;;)
	{
		if(!sr->inRange()) {
			throw BdfError(BdfError::ERROR_END_OF_FILE, *sr);
		}

		c = sr->upto[0];
		sr->upto += 1;

		if(c >= 'a' && c <= 'z') {
			c -= 32;
		}

		if(c == '.' || c == 'E') {
			isDecimal = true;
			number += c;
			continue;
		}

		if((c >= '0' && c <= '9') || c == '-' || c == '+') {
			number += c;
			continue;
		}
		
		try
		{
			switch(c)
			{
				case 'D':
					setDouble(std::stod(number));
					return;
				case 'F':
					setFloat(std::stof(number));
					return;
				case 'I':
					if(isDecimal) break;
					setInteger((int32_t)std::stol(number));
					return;
				case 'L':
					if(isDecimal) break;
					setLong((int64_t)std::stol(number));
					return;
				case 'S':
					if(isDecimal) break;
					setShort((int16_t)std::stoi(number));
					return;
				case 'B':
					if(isDecimal) break;
					setByte((char)std::stoi(number));
					return;
				default:
					throw BdfError(BdfError::ERROR_SYNTAX, sr->getPointer(-1));
			}
		}

		catch(std::invalid_argument &e) {
			throw BdfError(BdfError::ERROR_SYNTAX, sr->getPointer(-number.size() - 1), number.size() + 1);
		}

		catch(std::out_of_range &e) {
			throw BdfError(BdfError::ERROR_OUT_OF_RANGE, sr->getPointer(-number.size() - 1), number.size() + 1);
		}

		throw BdfError(BdfError::ERROR_SYNTAX, *sr);
	}
}

BdfObject::~BdfObject() {
	freeAll();
}

void BdfObject::freeAll()
{
	switch(type)
	{
		case BdfTypes::LIST:
		{
			if(object != NULL) {
				delete (BdfList*)object;
				object = NULL;
			}
		
			break;
		}
			
		case BdfTypes::NAMED_LIST:
		{
			if(object != NULL) {
				delete (BdfNamedList*)object;
				object = NULL;
			}
			
			break;
		}

		case BdfTypes::STRING:
		{
			if(object != NULL) {
				delete (std::string*)object;
				object = NULL;
			}

			break;
		}
	}

	if(data != NULL)
	{
		delete[] data;

		data = NULL;
	}

	type = BdfTypes::UNDEFINED;
}

char BdfObject::getType() {
	return type;
}

int BdfObject::serializeSeeker(int* locations)
{
	int size = getDefaultSize(type);
	
	if(size != -1) {
		last_seek = size;
		return size;
	}

	// Objects
	switch(type)
	{
		case BdfTypes::STRING:
			size = ((std::string*)object)->size() + 1;
			break;
		case BdfTypes::NAMED_LIST:
			size = ((BdfNamedList*)object)->serializeSeeker(locations) + 1;
			break;
		case BdfTypes::LIST:
			size = ((BdfList*)object)->serializeSeeker(locations) + 1;
			break;
		default:
			size = s + 1;
	}

	int size_bytes;

	if(size > 65531) {
		size_bytes = 4;
	} else if(size > 253) {
		size_bytes = 2;
	} else {
		size_bytes = 1;
	}

	size += size_bytes;
	last_seek = size;
		
	return size;
}

int BdfObject::serialize(char *pData, int* locations, unsigned char parent_flags)
{
	int size = last_seek;
	bool storeSize = shouldStoreSize(type);

	char size_bytes_tag = 0;
	int size_bytes = 0;

	if(storeSize)
	{
		if(size > 65535) {
			size_bytes_tag = 0;
			size_bytes = 4;
		} else if(size > 255) {
			size_bytes_tag = 1;
			size_bytes = 2;
		} else {
			size_bytes_tag = 2;
			size_bytes = 1;
		}
	}

	int offset = size_bytes + 1;
	unsigned char flags = (unsigned char)(type + (size_bytes_tag * 18) + (parent_flags * 3 * 18));
	
	// Objects
	switch(type)
	{
		case BdfTypes::STRING: {
			std::string* str = (std::string*)object;
			memcpy(pData + offset, str->c_str(), str->size());
			size = str->size() + offset;
			break;
		}
		case BdfTypes::NAMED_LIST: {
			BdfNamedList* v = (BdfNamedList*)object;
			size = v->serialize(pData + offset, locations) + offset;
			break;
		}
		case BdfTypes::LIST: {
			BdfList* v = (BdfList*)object;
			size = v->serialize(pData + offset, locations) + offset;
			break;
		}
		case BdfTypes::UNDEFINED: {
			break;
		}
		default: {
			size = s + offset;
			memcpy(pData + offset, data, s);
		}
	}

	pData[0] = flags;

	if(storeSize)
	{
		switch(size_bytes_tag)
		{
			case 0:
				put_netsi(pData + 1, size);
				break;
			case 1:
				put_netus(pData + 1, size);
				break;
			default:
				pData[1] = size & 255;
		}
	}

	return size;
}

void BdfObject::getLocationUses(int* locations)
{
	switch(type)
	{
		case BdfTypes::NAMED_LIST:
			((BdfNamedList*)object)->getLocationUses(locations);
			return;
		case BdfTypes::LIST:
			((BdfList*)object)->getLocationUses(locations);
			return;
		default:
			return;
	}
}

std::string calcIndent(BdfIndent indent, int it)
{
	std::string t = "";

	for(int i=0;i<=it;i++) {
		t += indent.indent;
	}

	return t;
}

void decimalToStream(std::ostream &out, double v)
{
	if(std::isnan(v)) {
		out << "NaN";
	}

	else if(std::isinf(v)) {
		out << ((v < 0) ? "-Infinity" : "Infinity");
	}

	else {
		out << v;
	}
}

void decimalToStream(std::ostream &out, float v)
{
	if(std::isnan(v)) {
		out << "NaN";
	}

	else if(std::isinf(v)) {
		out << ((v < 0) ? "-Infinity" : "Infinity");
	}

	else {
		out << v;
	}
}

void BdfObject::serializeHumanReadable(std::ostream &out, BdfIndent indent, int it)
{
	switch (type)
	{
		// Objects

		case BdfTypes::LIST: {
			((BdfList*)object)->serializeHumanReadable(out, indent, it);
			return;
		}

		case BdfTypes::NAMED_LIST: {
			((BdfNamedList*)object)->serializeHumanReadable(out, indent, it);
			return;
		}

		case BdfTypes::STRING: {
			out << serializeString(getString());
			return;
		}

		// Primitives

		case BdfTypes::INTEGER: {
			out << getInteger() << "I";
			return;
		}

		case BdfTypes::BOOLEAN: {
			if(getBoolean()) out << "true";
			else out << "false";
			return;
		}

		case BdfTypes::LONG: {
			out << getLong() << "L";
			return;
		}

		case BdfTypes::SHORT: {
			out << getShort() << "S";
			return;
		}

		case BdfTypes::BYTE: {
			out << (int)getByte() << "B";
			return;
		}

		case BdfTypes::DOUBLE:
		{
			decimalToStream(out, getDouble());
			out << "D";

			return;
		}

		case BdfTypes::FLOAT:
		{
			decimalToStream(out, getFloat());
			out << "F";

			return;
		}

		// Arrays

		case BdfTypes::ARRAY_INTEGER:
		{
			out << "int(";

			int32_t* v;
			int size;
			getIntegerArray(&v, &size);

			for(int i=0;i<size;i++) {
				out << indent.breaker << calcIndent(indent, it) << v[i] << "I";
				if(i != size - 1) out << ", ";
			}

			out << indent.breaker << calcIndent(indent, it - 1) << ")";

			delete[] v;
			return;
		}

		case BdfTypes::ARRAY_BOOLEAN:
		{
			out << "bool(";

			bool* v;
			int size;
			getBooleanArray(&v, &size);

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
			out << "long(";

			int64_t* v;
			int size;
			getLongArray(&v, &size);

			for(int i=0;i<size;i++) {
				out << indent.breaker << calcIndent(indent, it) << v[i] << "L";
				if(i != size - 1) out << ", ";
			}

			out << indent.breaker << calcIndent(indent, it - 1) << ")";

			delete[] v;
			return;
		}

		case BdfTypes::ARRAY_SHORT:
		{
			out << "short(";

			int16_t* v;
			int size;
			getShortArray(&v, &size);

			for(int i=0;i<size;i++) {
				out << indent.breaker << calcIndent(indent, it) << v[i] << "S";
				if(i != size - 1) out << ", ";
			}

			out << indent.breaker << calcIndent(indent, it - 1) << ")";

			delete[] v;
			return;
		}

		case BdfTypes::ARRAY_BYTE:
		{
			out << "byte(";

			char* v;
			int size;
			getByteArray(&v, &size);

			for(int i=0;i<size;i++) {
				out << indent.breaker << calcIndent(indent, it) << (int)v[i] << "B";
				if(i != size - 1) out << ", ";
			}

			out << indent.breaker << calcIndent(indent, it - 1) << ")";

			delete[] v;
			return;
		}

		case BdfTypes::ARRAY_DOUBLE:
		{
			out << "double(";

			double* v;
			int size;
			getDoubleArray(&v, &size);

			for(int i=0;i<size;i++)
			{
				out << indent.breaker << calcIndent(indent, it);
				
				decimalToStream(out, v[i]);
				
				out << "D";
				
				if(i != size - 1) out << ", ";
			}

			out << indent.breaker << calcIndent(indent, it - 1) << ")";

			delete[] v;
			return;
		}

		case BdfTypes::ARRAY_FLOAT:
		{
			out << "float(";

			float* v;
			int size;
			getFloatArray(&v, &size);

			for(int i=0;i<size;i++)
			{
				out << indent.breaker << calcIndent(indent, it);
				
				decimalToStream(out, v[i]);
				
				out << "F";
				
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



int BdfObject::getKeyLocation(std::string key) {
	return lookupTable->getLocation(key);
}

std::string BdfObject::getKeyName(int key) {
	return lookupTable->getName(key);
}

BdfObject* BdfObject::newObject() {
	return new BdfObject(lookupTable);
}

BdfNamedList* BdfObject::newNamedList() {
	return new BdfNamedList(lookupTable);
}

BdfList* BdfObject::newList() {
	return new BdfList(lookupTable);
}

BdfObject* BdfObject::setAutoInt(long number)
{
	if(number > 2147483648L || number <= -2147483648L) {
		setLong(number);
	} else if(number > 32768 || number <= -32768) {
		setInteger((int)number);
	} else if(number > 128 || number <= -128) {
		setShort((short)number);
	} else {
		setByte((char)number);
	}
	
	return this;
}

long BdfObject::getAutoInt()
{
	switch(type)
	{
		case BdfTypes::BYTE:
			return getByte();
		case BdfTypes::SHORT:
			return getShort();
		case BdfTypes::INTEGER:
			return getInteger();
		case BdfTypes::LONG:
			return getLong();
		default:
			return 0;
	}
}

// Get

// Primitives

int32_t BdfObject::getInteger()
{
	if(type != BdfTypes::INTEGER) {
		return 0;
	}

	return get_netsi(data);
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

	return get_netsl(data);
}

int16_t BdfObject::getShort()
{
	if(type != BdfTypes::SHORT) {
		return 0;
	}

	return get_netss(data);
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

	return get_netd(data);
}

float BdfObject::getFloat()
{
	if(type != BdfTypes::FLOAT) {
		return 0;
	}

	return get_netf(data);
}

// Arrays

void BdfObject::getIntegerArray(int32_t** v, int* pSize)
{
	if(type != BdfTypes::ARRAY_INTEGER) {
		*v = new int32_t[0];
		*pSize = 0;
	}

	int l = sizeof(int32_t);
	int size = s / l;
	int32_t* array = new int32_t[size];
	*v = array;
	*pSize = size;

	for(int i=0;i<size;i++) {
		array[i] = get_netsi(data + i * l);
	}
}

void BdfObject::getBooleanArray(bool** v, int* pSize)
{
	if(type != BdfTypes::ARRAY_BOOLEAN) {
		*v = new bool[0];
		*pSize = 0;
	}

	*pSize = s;
	*v = new bool[s];
	bool* bytes = *v;

	for(int i=0;i<s;i++) {
		bytes[i] = (data[i] == 0x01 ? true : false);
	}
}

void BdfObject::getShortArray(int16_t** v, int* pSize)
{
	if(type != BdfTypes::ARRAY_SHORT) {
		*v = new int16_t[0];
		*pSize = 0;
	}

	int l = sizeof(int16_t);
	int size = s / l;
	int16_t* array = new int16_t[size];
	*pSize = size;
	*v = array;

	for(int i=0;i<size;i++) {
		array[i] = get_netss(data + i * l);
	}
}

void BdfObject::getLongArray(int64_t** v, int* pSize)
{
	if(type != BdfTypes::ARRAY_LONG) {
		*v = new int64_t[0];
		*pSize = 0;
	}

	int l = sizeof(int64_t);
	int size = s / l;
	int64_t* array = new int64_t[size];
	*pSize = size;
	*v = array;

	for(int i=0;i<size;i++) {
		array[i] = get_netsl(data + i * l);
	}
}

void BdfObject::getByteArray(char** v, int* pSize)
{
	if(type != BdfTypes::ARRAY_BYTE) {
		*v = new char[0];
		*pSize = 0;
	}

	*pSize = s;
	*v = new char[s];
	memcpy(*v, data, s);
}

void BdfObject::getDoubleArray(double** v, int* pSize)
{
	if(type != BdfTypes::ARRAY_DOUBLE) {
		*v = new double[0];
		*pSize = 0;
	}

	int l = sizeof(double);
	int size = s / l;
	double* array = new double[size];
	*v = array;
	*pSize = size;

	for(int i=0;i<size;i++) {
		array[i] = get_netd(data + i * l);
	}
}

void BdfObject::getFloatArray(float** v, int* pSize)
{
	if(type != BdfTypes::ARRAY_FLOAT) {
		*v = new float[0];
		*pSize = 0;
	}

	int l = sizeof(float);
	int size = s / l;
	float* array = new float[size];
	*v = array;
	*pSize = size;

	for(int i=0;i<size;i++) {
		array[i] = get_netf(data + i * l);
	}
}

// Objects

std::string BdfObject::getString()
{
	std::string* v;

	if(type == BdfTypes::STRING) {
		v = (std::string*)object;
	}
	
	else
	{
		freeAll();
		v = new std::string();
	}

	type = BdfTypes::STRING;
	object = v;
	return *v;
}

BdfList* BdfObject::getList()
{
	BdfList* v;

	if(type == BdfTypes::LIST) {
		v = (BdfList*)object;
	}
	
	else
	{
		freeAll();
		v = new BdfList(lookupTable);
	}

	type = BdfTypes::LIST;
	object = v;
	return v;
}

BdfNamedList* BdfObject::getNamedList()
{
	BdfNamedList* v;

	if(type == BdfTypes::NAMED_LIST) {
		v = (BdfNamedList*)object;
	}
	
	else
	{
		freeAll();
		v = new BdfNamedList(lookupTable);
	}

	type = BdfTypes::NAMED_LIST;
	object = v;
	return v;
}

// Set

// Primitives

BdfObject* BdfObject::setInteger(int32_t v)
{
	freeAll();

	s = sizeof(v);
	data = new char[4];
	type = BdfTypes::INTEGER;
	put_netsi(data, v);
	return this;
}

BdfObject* BdfObject::setLong(int64_t v)
{
	freeAll();

	s = sizeof(v);
	data = new char[8];
	type = BdfTypes::LONG;
	put_netsl(data, v);
	return this;
}

BdfObject* BdfObject::setShort(int16_t v)
{
	freeAll();

	s = sizeof(v);
	data = new char[2];
	type = BdfTypes::SHORT;
	put_netss(data, v);
	return this;
}

BdfObject* BdfObject::setBoolean(bool v)
{
	freeAll();

	s = 1;
	data = new char[1] {(char)(v ? 0x01 : 0x00)};
	type = BdfTypes::BOOLEAN;
	return this;
}

BdfObject* BdfObject::setDouble(double v)
{
	freeAll();

	s = sizeof(v);
	data = new char[8];
	type = BdfTypes::DOUBLE;
	put_netd(data, v);
	return this;
}

BdfObject* BdfObject::setFloat(float v)
{
	freeAll();

	s = sizeof(v);
	data = new char[4];
	type = BdfTypes::FLOAT;
	put_netf(data, v);
	return this;
}

BdfObject* BdfObject::setByte(char v)
{
	freeAll();

	s = sizeof(v);
	type = BdfTypes::BYTE;
	data = new char[1] {v};
	return this;
}

// Arrays

BdfObject* BdfObject::setIntegerArray(const int32_t* v, int size)
{
	freeAll();

	s = 4 * size;
	data = new char[s];
	type = BdfTypes::ARRAY_INTEGER;

	for(int i=0;i<size;i++) {
		put_netsi(data + i * 4, v[i]);
	}

	return this;
}

BdfObject* BdfObject::setBooleanArray(const bool* v, int size)
{
	freeAll();

	s = size;
	data = new char[s];
	type = BdfTypes::ARRAY_BOOLEAN;

	for(int i=0;i<s;i++) {
		data[i] = (char)(v[i] ? 0x01 : 0x00);
	}

	return this;
}

BdfObject* BdfObject::setLongArray(const int64_t* v, int size)
{
	freeAll();

	s = 8 * size;
	data = new char[s];
	type = BdfTypes::ARRAY_LONG;

	for(int i=0;i<size;i++) {
		put_netsl(data + i * 8, v[i]);
	}

	return this;
}

BdfObject* BdfObject::setShortArray(const int16_t* v, int size)
{
	freeAll();

	s = 2 * size;
	data = new char[s];
	type = BdfTypes::ARRAY_SHORT;

	for(int i=0;i<size;i++) {
		put_netss(data + i * 2, v[i]);
	}

	return this;
}

BdfObject* BdfObject::setByteArray(const char* v, int size)
{
	freeAll();

	s = size;
	type = BdfTypes::ARRAY_BYTE;

	data = new char[s];
	memcpy(data, v, size);

	return this;
}

BdfObject* BdfObject::setDoubleArray(const double* v, int size)
{
	freeAll();

	s = 8 * size;
	data = new char[s];
	type = BdfTypes::ARRAY_DOUBLE;

	for(int i=0;i<size;i++) {
		put_netd(data + i * 8, v[i]);
	}

	return this;
}

BdfObject* BdfObject::setFloatArray(const float* v, int size)
{
	freeAll();

	s = 4 * size;
	data = new char[s];
	type = BdfTypes::ARRAY_FLOAT;

	for(int i=0;i<size;i++) {
		put_netf(data + i * 4, v[i]);
	}

	return this;
}

// Objects

BdfObject* BdfObject::setString(std::string v)
{
	freeAll();

	type = BdfTypes::STRING;
	object = new std::string(v);

	return this;
}

BdfObject* BdfObject::setList(BdfList* v)
{
	freeAll();

	type = BdfTypes::LIST;
	object = v;

	return this;
}

BdfObject* BdfObject::setNamedList(BdfNamedList* v)
{
	freeAll();

	type = BdfTypes::NAMED_LIST;
	object = v;

	return this;
}
