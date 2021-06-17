
#include "Bdf.hpp"
#include "BdfHelpers.hpp"
#include <vector>
#include <cstdint>
#include <iostream>
#include <string.h>

using namespace Bdf;
using namespace BdfHelpers;

BdfList::BdfList(BdfLookupTable* lookupTable, const char* data, int size)
{
	this->start = NULL;
	this->end = NULL;
	this->endptr = &this->start;
		
	int i = 0;

	while(i < size)
	{
		char object_size_tag;
		BdfObject::getFlagData(data, NULL, &object_size_tag, NULL);

		char object_size_bytes = BdfObject::getSizeBytes(object_size_tag);
		
		if(i + object_size_bytes >= size) {
			return;
		}

		// Get the size of the object
		int object_size = BdfObject::getSize(data + i);
	
		if(object_size <= 0 || i + object_size > size) {
			return;
		}

		// Add the object to the elements list
		add(new BdfObject(lookupTable, data + i, object_size));

		// Increase the iterator by the amount of bytes
		i += object_size;
	}
}

BdfList::BdfList(BdfLookupTable* lookupTable) : BdfList(lookupTable, NULL, 0) {

}

BdfList::BdfList(BdfLookupTable* lookupTable, BdfStringReader* sr)
{
	this->start = NULL;
	this->end = NULL;
	this->endptr = &this->start;
		
	sr->upto += 1;

	// [..., ...]

	try
	{	
		for(;;)
		{
			sr->ignoreBlanks();

			if(sr->upto[0] == ']') {
				sr->upto += 1;
				return;
			}

			BdfObject* bdf = new BdfObject(lookupTable, sr);
			add(bdf);

			// There should be a comma after this
			sr->ignoreBlanks();
			wchar_t c = sr->upto[0];
	
			if(c == ']') {
				sr->upto += 1;
				return;
			}

			if(c != ',') {
				throw BdfError(BdfError::ERROR_SYNTAX, *sr);
			}

			sr->upto += 1;
			sr->ignoreBlanks();
		}
	}

	catch(BdfError &e)
	{
		clear();
		
		throw;
	}
}

BdfList::~BdfList()
{
	clear();
}

int BdfList::size()
{
	int size = 0;
	Item* upto = this->start;

	while(upto != NULL)
	{
		size++;
		upto = upto->next;
	}

	return size;
}

BdfList::Item* BdfList::getAtIndex(int index)
{
	Item* upto = this->start;

	if(index < 0)
	{
		throw std::out_of_range("index " + std::to_string(index) + " is out of range");
	}

	while(index != 0)
	{
		index--;
		upto = upto->next;

		if(upto == NULL)
		{
			throw std::out_of_range("index " + std::to_string(index) + " is out of range");
		}
	}

	return upto;
}

BdfObject* BdfList::get(int index)
{
	return getAtIndex(index)->object;
}

BdfList* BdfList::set(int index, BdfObject* o)
{
	Item* item = getAtIndex(index);

	delete item->object;
	item->object = o;

	return this;
}

BdfList::Item* BdfList::getStart()
{
	return this->start;
}

BdfList::Item* BdfList::getEnd()
{
	return this->end;
}

BdfList* BdfList::insertNext(Item* item, BdfObject* object)
{
	if(item->next == NULL)
	{
		add(object);
	}

	else
	{
		Item* item_new = new Item();

		item_new->object = object;
		item_new->last = item;
		item_new->next = item->next;

		item->next->last = item_new;
		item->next = item_new;
	}

	return this;
}

BdfList* BdfList::insertLast(Item* item, BdfObject* object)
{
	Item* item_new = new Item();

	item_new->object = object;

	if(item->last == NULL)
	{
		item_new->last = NULL;
		item_new->next = item;

		item->last = item_new;
		this->start = item_new;
	}

	else
	{
		item_new->next = item;
		item_new->last = item->last;
		
		item->last->next = item_new;
		item->last = item_new;
	}

	return this;
}

BdfList* BdfList::remove(Item* item)
{
	if(item->next == NULL && item->last == NULL)
	{
		this->start = NULL;
		this->end = NULL;
		this->endptr = &this->start;
	}

	else if(item->last == NULL)
	{
		item->next->last = NULL;
		this->start = item->next;
	}

	else if(item->next == NULL)
	{
		item->last->next = NULL;
		this->end = item->last;
		this->endptr = &item->last->next;
	}

	else
	{
		item->next->last = item->last;
		item->last->next = item->next;
	}

	BdfObject* object = item->object;

	delete item;
	delete object;

	return this;
}

BdfList* BdfList::remove(BdfObject* item)
{
	Item* cur = this->start;

	while(cur != NULL && cur->object != item)
	{
		cur = cur->next;
	}

	if(cur != NULL)
	{
		remove(cur);
	}
	
	return this;
}

BdfList* BdfList::add(BdfObject* o)
{
	Item* item = new Item();
	
	item->object = o;
	item->last = this->end;
	item->next = NULL;

	*this->endptr = item;
	this->endptr = &item->next;
	this->end = item;
		
	return this;
}

BdfList* BdfList::remove(int index)
{
	return remove(getAtIndex(index));
}

BdfList* BdfList::clear()
{
	Item* upto = this->start;
	Item* next;

	while(upto != NULL)
	{
		next = upto->next;

		delete upto->object;
		delete upto;

		upto = next;
	}

	return this;
}

void BdfList::getLocationUses(int* locations)
{
	Item* upto = this->start;

	while(upto != NULL)
	{
		upto->object->getLocationUses(locations);
		upto = upto->next;
	}
}

int BdfList::serializeSeeker(int* locations)
{
	Item* upto = this->start;
	int size = 0;

	while(upto != NULL)
	{
		size += upto->object->serializeSeeker(locations);
		upto = upto->next;
	}

	return size;
}

int BdfList::serialize(char *data, int* locations)
{
	Item* upto = this->start;
	int pos = 0;

	while(upto != NULL)
	{
		pos += upto->object->serialize(data + pos, locations, 0);
		upto = upto->next;
	}

	return pos;
}

void BdfList::serializeHumanReadable(std::ostream &out, BdfIndent indent, int it)
{
	if(this->start == NULL)
	{
		out << "[]";
		
		return;
	}

	out << "[";

	Item* upto = this->start;

	if(upto != NULL)
	{
		for(;;)
		{
			BdfObject *o = upto->object;
	
			out << indent.breaker;
	
			for(int n=0;n<=it;n++) {
				out << indent.indent;
			}
	
			o->serializeHumanReadable(out, indent, it + 1);
			upto = upto->next;
	
			if(upto != NULL)
			{
				out << ", ";
			}

			else
			{
				break;
			}
		}
	}

 	out << indent.breaker;

	for(int n=0;n<it;n++) {
		out << indent.indent;
	}

	out << "]";
}


