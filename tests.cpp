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
#include <unistd.h>

int main()
{
	BdfObject* bdf = new BdfObject();
	BdfObject* bdf_last = bdf;

	// Allocate a MB of data
	int s = 1024*1024*1024;
	char* bigData = new char[s];

	for(int i=0;i<s;i++) {
		bigData[i] = (char)0;
	}

	for(int i=0;i<10;i++)
	{
		BdfArray* a = bdf_last->getArray();
		bdf_last = new BdfObject();
		a->add(bdf_last);
	}

	bdf_last->getArray()->add((new BdfObject())->setByteArray(bigData, s));

	delete[] bigData;

	//bdf->serializeHumanReadable(std::cout, BdfIndent("  ", "\n"));

	char* bytes;
	int size = bdf->serialize(&bytes);

	bdf->freeAll();

	bdf = new BdfObject(bytes, size);
	delete[] bytes;

	bdf->freeAll();

	//bdf->serializeHumanReadable(std::cout, BdfIndent("  ", "\n"));

	std::cout << "1\n";

	sleep(5);

	std::cout << "2\n";

	return 0;
}
