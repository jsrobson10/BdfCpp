/*
 * bdfobject.h
 *
 *  Created on: 30 May 2020
 *      Author: josua
 */

#ifndef BDFOBJECT_H_
#define BDFOBJECT_H_

#include "headers.h"
#include <iostream>
#include <string>

class BdfObject
{
	private:

	void *object;
	char type;
	char *data;
	int s;

	public:

	BdfObject();
	BdfObject(char *data, int size);
	virtual ~BdfObject();

	char getType();
	int _serializeSeek();
	int _serialize(char *data);
	int serialize(char **data);

	void serializeHumanReadable(std::ostream &stream, BdfIndent indent, int upto);
	void serializeHumanReadable(std::ostream &stream, BdfIndent indent);
	void serializeHumanReadable(std::ostream &stream);
	std::string serializeHumanReadable(BdfIndent indent);
	std::string serializeHumanReadable();
	void freeAll();

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
	int getIntegerArray(int32_t **v);
	int getBooleanArray(bool **v);
 	int getLongArray(int64_t **v);
 	int getShortArray(int16_t **v);
 	int getByteArray(char **v);
	int getDoubleArray(double **v);
	int getFloatArray(float **v);

	// Objects
	std::string getString();
	BdfArray* getArray();
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
	BdfObject* setIntegerArray(int32_t *v, int size);
	BdfObject* setBooleanArray(bool *v, int size);
 	BdfObject* setLongArray(int64_t *v, int size);
 	BdfObject* setShortArray(int16_t *v, int size);
 	BdfObject* setByteArray(char *v, int size);
	BdfObject* setDoubleArray(double *v, int size);
	BdfObject* setFloatArray(float *v, int size);

	// Objects
	BdfObject* setString(std::string v);
	BdfObject* setArray(BdfArray* v);
	BdfObject* setNamedList(BdfNamedList* v);
};

#endif /* BDFOBJECT_H_ */
