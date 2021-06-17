
#ifndef BDFLOOKUPTABLE_HPP_
#define BDFLOOKUPTABLE_HPP_

#include "Bdf.hpp"
#include <iostream>
#include <vector>
#include <string>

namespace Bdf
{
	class BdfLookupTable
	{
	private:

		class Item;

		class Item
		{
		public:
			std::string key;
			Item* next;
		};

		Item* keys_start;
		Item** keys_endp;
		Item** keys_mapped;
		unsigned int keys_size_mapped;
		unsigned int keys_size;

		BdfReader* reader;

		void remapKeys();
	
	public:
		BdfLookupTable(BdfReader* reader);
		BdfLookupTable(BdfReader* reader, const char* data, int size);
		virtual ~BdfLookupTable();
		unsigned int getLocation(std::string id);
		std::string getName(unsigned int id);
		int serialize(char* database, int* locations, int locations_size);
		int serializeSeeker(int* locations, int locations_size);
		void serializeGetLocations(int* locations);
		bool hasKeyLocation(unsigned int key);
		int size();
	};
}

#endif
