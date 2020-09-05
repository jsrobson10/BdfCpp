
#ifndef BDFARRAY_H_
#define BDFARRAY_H_

#include "Bdf.h"
#include "BdfHelpers.h"
#include <iostream>
#include <vector>

class BdfArray
{
private:
	std::vector<BdfObject*> objects;

public:
	BdfArray(BdfLookupTable* lookupTable);
	BdfArray(BdfLookupTable* lookupTable, const char* data, int size);
	BdfArray(BdfLookupTable* lookupTable, BdfStringReader* sr);
	
	virtual ~BdfArray();

	static BdfNamedList readHumanReadable(BdfLookupTable* lookupTable, char* data, int size);

	void serializeHumanReadable(std::ostream &stream, BdfIndent indent, int upto);
	void getLocationUses(int* locations);
	int serializeSeeker(int* locations);
	int serialize(char *data, int* locations);
	
	BdfArray* add(BdfObject* o);
	BdfArray* clear();
	BdfObject* remove(int index);
	BdfObject* get(int index);
	BdfArray* set(int index, BdfObject* o);
	int size();
};

#endif /* BDFARRAY_H_ */
