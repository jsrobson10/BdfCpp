/*
 * tests.cpp
 *
 *  Created on: 31 May 2020
 *      Author: josua
 */

#include <fstream>
#include <iostream>
#include "BdfObject.h"
#include "BdfArray.h"

int main()
{
	BdfObject* bdf = new BdfObject();
	BdfNamedList* nl = bdf->getNamedList();
	BdfArray* array = new BdfArray();

	double* d_array = new double[4] {
		1.2, 6.9, 42.0, 8,
	};

	array->add((new BdfObject())->setFloat(42.0f));
	array->add((new BdfObject())->setDouble(420.0));
	array->add((new BdfObject())->setInteger(123));
	array->add((new BdfObject()));
	array->add((new BdfObject())->setBoolean(false));
	array->add((new BdfObject())->setBoolean(true));
	array->add((new BdfObject())->setDoubleArray(d_array, 4));

	nl->set("age", (new BdfObject())->setDouble(23));
	nl->set("array", (new BdfObject())->setArray(array));
	nl->set("value", (new BdfObject())->setLong(45));

	std::cout << bdf->serializeHumanReadable() << "\n";

	char* bytes;
	int size = bdf->serialize(&bytes);

	BdfObject* bdf2 = new BdfObject(bytes, size);

	std::cout << bdf2->serializeHumanReadable(BdfIndent("  ", "\n")) << "\n";

	//std::cout << bdf2->getNamedList()->get("array")->getArray()->get(0)->getFloat() << "\n";

	return 0;
}
