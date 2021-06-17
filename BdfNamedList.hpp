
#ifndef BDFNAMEDLIST_HPP_
#define BDFNAMEDLIST_HPP_

#include "Bdf.hpp"
#include <iostream>
#include <vector>
#include <string>

namespace Bdf
{
	
	class BdfNamedList
	{
	private:
	
		class Item;

		class Item
		{
		public:
			Item* next;
			BdfObject* object;
			int key;
		
			Item(int key, BdfObject* object, Item* next);
			virtual ~Item();
		};
		
		Item* start;
		Item** end;

		BdfLookupTable* lookupTable;

	public:
		BdfNamedList(BdfLookupTable* lookupTable);
		BdfNamedList(BdfLookupTable* lookupTable, const char* data, int size);
		BdfNamedList(BdfLookupTable* lookupTable, BdfStringReader* sr);
	
		virtual ~BdfNamedList();
	
		void getLocationUses(int* locations);
		int serializeSeeker(int* locations);
		int serialize(char *data, int* locations);
		void serializeHumanReadable(std::ostream &stream, BdfIndent indent, int upto);

		BdfNamedList* clear();	
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
}

#endif
