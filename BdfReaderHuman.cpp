
#include "Bdf.hpp"
#include "BdfHelpers.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <codecvt>
#include <locale>

using namespace Bdf;
using namespace BdfHelpers;

BdfReaderHuman::BdfReaderHuman(std::wstring data)
{
	BdfStringReader sr(data.c_str(), data.size());
	sr.ignoreBlanks();

	BdfObject* bdfNew = new BdfObject(lookupTable, &sr);

	try {
		sr.ignoreBlanks();
	}

	catch(BdfError &e)
	{
		if(e.getType() != BdfError::ERROR_END_OF_FILE)
		{
			delete bdfNew;
			throw;
		}

		delete bdf;
		bdf = bdfNew;

		return;
	}

	delete bdfNew;
	throw BdfError(BdfError::ERROR_SYNTAX, sr);
}

BdfReaderHuman::BdfReaderHuman(std::string data) : BdfReaderHuman(
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(data)) {
}
