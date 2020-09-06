
#include <bdf/Bdf.h>

#include <iostream>
#include <fstream>

/*
 * fibonacci.cpp
 * Made by Josua Robson
 *
 * This example shows use of primitive arrays
 * as well as serialization to binary.
 * 
 * This example stores the result in a
 * file "fibonacci.bdf". To read this
 * file you can use the included tools,
 * "bdfconvert" and/or "bdfedit".
 *
 * Read fibbonacci.bdf with:
 * cat fibbonacci.bdf | bdfconvert
 *
 * Compile this example with:
 * g++ fibonacci.cpp -o fibonacci -lbdf
 *
 */

int main()
{
	// Generate a fibonacci sequence with 20 elements
	
	int size = 20;
	int* fibonacci = new int[size];
	
	int a = 0;
	int b = 1;

	for(int i=0;i<size;i++)
	{
		fibonacci[i] = b;
			
		int c = a + b;
		a = b;
		b = c;
	}

	
	// Pack the fibonacci sequence into a BDF archive
	
	BdfReader reader;

	// Set the reader object to a named list
	BdfObject* bdf = reader.getObject();
	BdfNamedList* nl = bdf->getNamedList();

	// Set "fibonacciNumbers" in the named list to the fibonacci numbers as an int array
	nl->set("fibonacciNumbers", bdf->newObject()->setIntegerArray(fibonacci, size));

	delete[] fibonacci;

	// Serialize the fibonacci sequence
	char* data;
	int data_size;
	reader.serialize(&data, &data_size);

	
	// Store the serialized fibonacci sequence into a file

	std::ofstream out("./fibonacci.bdf", std::ios::binary);

	for(int i=0;i<data_size;i+=1024) {
		out.write(data, std::min(1024, data_size - i));
	}

	delete[] data;

	out.close();
}
