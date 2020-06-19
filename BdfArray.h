/*
 * BdfArray.h
 *
 *  Created on: 31 May 2020
 *      Author: josua
 */

#ifndef BDFARRAY_H_
#define BDFARRAY_H_

#include "headers.h"
#include <iostream>
#include <vector>

class BdfArray
{
private:
	std::vector<BdfObject*> objects;

public:
	BdfArray();
	BdfArray(char data[], int size);
	virtual ~BdfArray();
	int _serializeSeek();
	int _serialize(char *data);
	std::string serializeHumanReadable(BdfIndent indent, int upto);
	BdfArray* add(BdfObject *o);
	BdfArray* clear();
	BdfObject* remove(int index);
	BdfObject* get(int index);
	BdfArray* set(int index, BdfObject *o);
	int size();

	void serializeHumanReadable(std::ostream &stream, BdfIndent indent, int upto);
	void freeAll();
};

#endif /* BDFARRAY_H_ */
