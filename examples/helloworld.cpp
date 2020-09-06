
#include <bdf/Bdf.h>

#include <iostream>

/*
 * helloworld.cpp
 * Made by Josua Robson
 *
 * This example shows the use of generating
 * a bdf structure and serializing it to a file.
 *
 * The output of this command should just be:
 * {"Hello", "World!"}
 *
 * Compile this example with:
 * g++ helloworld.cpp -o helloworld -lbdf
 *
 */

int main()
{
	BdfReader reader;

	// Create a new named list and assign it to the reader object
	BdfObject* bdf = reader.getObject();
	BdfNamedList* nl = bdf->getNamedList();

	// Set the "Hello" tag in the named list to a string
	nl->set("Hello", bdf->newObject()->setString("World!"));

	// Serialize the reader and send it to the console
	reader.serializeHumanReadable(std::cout);

	return 0;
}
