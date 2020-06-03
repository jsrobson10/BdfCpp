/*
 * BdfNamedList.h
 *
 *  Created on: 31 May 2020
 *      Author: josua
 */

#ifndef BDFNAMEDLIST_H_
#define BDFNAMEDLIST_H_

#include "headers.h"
#include <vector>
#include <string>

class ListObject
{
public:

	BdfObject *object;
	std::string key;

	ListObject(std::string key, BdfObject* object);
	virtual ~ListObject();
};

class BdfNamedList
{
private:
	std::vector<ListObject> objects;

public:
	BdfNamedList();
	BdfNamedList(char data[], int size);
	virtual ~BdfNamedList();
	int serialize(char **data);
	std::string serializeHumanReadable(BdfIndent indent, int upto);

	BdfObject* get(std::string key);
	BdfNamedList* set(std::string key, BdfObject* value);
	BdfObject* remove(std::string key);
	std::vector<std::string> keys();
	bool exists(std::string key);
};

#endif /* BDFNAMEDLIST_H_ */
