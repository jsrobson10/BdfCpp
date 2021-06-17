
#ifndef BDFOBJECT_HPP_
#define BDFOBJECT_HPP_

#include "Bdf.hpp"
#include <iostream>
#include <string>

namespace Bdf
{
	class BdfObject
	{
	private:
	
		BdfLookupTable* lookupTable;
		int last_seek;
		void *object;
		char type;
		char *data;
		int s;
	
		void freeAll();
	
	public:
	
		BdfObject(BdfLookupTable* lookupTable);
		BdfObject(BdfLookupTable* lookupTable, const char *data, int size);
		BdfObject(BdfLookupTable* lookupTable, BdfStringReader* sr);
	
		virtual ~BdfObject();
	
		char getType();
		void getLocationUses(int* locations);
		int serializeSeeker(int* locations);
		int serialize(char* data, int* locations, unsigned char flags);
		void serializeHumanReadable(std::ostream &stream, BdfIndent indent, int upto);
	
		static void getFlagData(const char* data, char* type, char* size_bytes, char* parent_flags);
		static char getSizeBytes(char size_bytes);
		static int getSize(const char* data);
		
		int getKeyLocation(std::string key);
		std::string getKeyName(int key);
	
		BdfObject* newObject();
		BdfNamedList* newNamedList();
		BdfList* newList();
	
		BdfObject* setAutoInt(long v);
		long getAutoInt();
	
		// Get
	
		// Primitives
		int32_t getInteger();
		bool getBoolean();
	 	int64_t getLong();
	 	int16_t getShort();
	 	char getByte();
		double getDouble();
		float getFloat();
	
		// Arrays
		void getIntegerArray(int32_t **v, int* s);
		void getBooleanArray(bool **v, int* s);
	 	void getLongArray(int64_t **v, int* s);
	 	void getShortArray(int16_t **v, int* s);
	 	void getByteArray(char **v, int* s);
		void getDoubleArray(double **v, int* s);
		void getFloatArray(float **v, int* s);
	
		// Objects
		std::string getString();
		BdfList* getList();
		BdfNamedList* getNamedList();
	
		// Set
	
		// Primitives
		BdfObject* setInteger(int32_t v);
		BdfObject* setBoolean(bool v);
	 	BdfObject* setLong(int64_t v);
	 	BdfObject* setShort(int16_t v);
	 	BdfObject* setByte(char v);
		BdfObject* setDouble(double v);
		BdfObject* setFloat(float v);
	
		// Arrays
		BdfObject* setIntegerArray(const int32_t *v, int size);
		BdfObject* setBooleanArray(const bool *v, int size);
	 	BdfObject* setLongArray(const int64_t *v, int size);
	 	BdfObject* setShortArray(const int16_t *v, int size);
	 	BdfObject* setByteArray(const char *v, int size);
		BdfObject* setDoubleArray(const double *v, int size);
		BdfObject* setFloatArray(const float *v, int size);
	
		// Objects
		BdfObject* setString(std::string v);
		BdfObject* setList(BdfList* v);
		BdfObject* setNamedList(BdfNamedList* v);
	};
}

#endif
