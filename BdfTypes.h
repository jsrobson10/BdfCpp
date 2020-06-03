/*
 * BdfTypes.h
 *
 *  Created on: 30 May 2020
 *      Author: josua
 */

#ifndef BDFTYPES_H_
#define BDFTYPES_H_

class BdfTypes
{
	public:

	const static char BOOLEAN = 0;
	const static char INTEGER = 1;
	const static char LONG = 2;
	const static char SHORT = 3;
	const static char BYTE = 4;
	const static char DOUBLE = 5;
	const static char FLOAT = 6;

	const static char STRING = 7;
	const static char ARRAY = 8;
	const static char NAMED_LIST = 9;
	const static char EMPTY = 10;

	const static char ARRAY_BOOLEAN = 11;
	const static char ARRAY_INTEGER = 12;
	const static char ARRAY_LONG = 13;
	const static char ARRAY_SHORT = 14;
	const static char ARRAY_BYTE = 15;
	const static char ARRAY_DOUBLE = 16;
	const static char ARRAY_FLOAT = 17;
};

#endif /* BDFTYPES_H_ */
