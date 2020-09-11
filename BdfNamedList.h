
#ifndef BDFNAMEDLIST_H_
#define BDFNAMEDLIST_H_

#include "Bdf.h"
#include <iostream>
#include <vector>
#include <string>

class BdfNamedListObject
{
public:
	BdfObject* object;
	int key;

	BdfNamedListObject(int key, BdfObject* object);
	virtual ~BdfNamedListObject();
};

class BdfNamedList
{
private:
	std::vector<BdfNamedListObject> objects;
	BdfLookupTable* lookupTable;

public:
	BdfNamedList(BdfLookupTable* lookupTable);
	BdfNamedList(BdfLookupTable* lookupTable, const char* data, int size);
	BdfNamedList(BdfLookupTable* lookupTable, BdfStringReader* sr);

	virtual ~BdfNamedList();
	static BdfNamedList readHumanReadable(BdfLookupTable* lookupTable, char* data, int size);

	void getLocationUses(int* locations);
	int serializeSeeker(int* locations);
	int serialize(char *data, int* locations);
	void serializeHumanReadable(std::ostream &stream, BdfIndent indent, int upto);

	BdfObject* get(int key);
	BdfObject* get(std::string key);
	BdfNamedList* set(std::string key, BdfObject* value);
	BdfNamedList* set(int key, BdfObject* value);
	BdfObject* remove(std::string key);
	BdfObject* remove(int key);
	std::vector<int> keys();
	bool exists(std::string key);
	bool exists(int key);
};

#endif /* BDFNAMEDLIST_H_ */
