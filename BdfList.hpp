
#ifndef BDFLIST_HPP_
#define BDFLIST_HPP_

#include "Bdf.hpp"
#include "BdfHelpers.hpp"
#include <iostream>
#include <vector>

namespace Bdf
{
	class BdfList
	{
	public:

		class Item;

		class Item
		{
		public:

			BdfObject* object;
			Item* last;
			Item* next;
		};

		BdfList(BdfLookupTable* lookupTable);
		BdfList(BdfLookupTable* lookupTable, const char* data, int size);
		BdfList(BdfLookupTable* lookupTable, BdfStringReader* sr);
		
		virtual ~BdfList();
	
		void serializeHumanReadable(std::ostream &stream, BdfIndent indent, int upto);
		void getLocationUses(int* locations);
		int serializeSeeker(int* locations);
		int serialize(char *data, int* locations);
		
		BdfList* add(BdfObject* o);
		BdfList* clear();
		
		Item* getStart();
		Item* getEnd();

		BdfList* insertNext(Item* item, BdfObject* object);
		BdfList* insertLast(Item* item, BdfObject* object);
		
		BdfObject* get(int id);
		BdfList* remove(int id);
		BdfList* remove(Item* item);
		BdfList* remove(BdfObject* object);
		BdfList* set(int id, BdfObject* object);
		int size();

	private:

		Item* getAtIndex(int index);

		Item* start;
		Item* end;
		Item** endptr;
	};
}

#endif
