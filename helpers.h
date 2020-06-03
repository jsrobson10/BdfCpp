
#ifndef HELPERS_H_
#define HELPERS_H_

#include <string>

char* reverseIfLittleEndian(void* array, int size);
char* reverseArray(void* array, int size);
bool isLittleEndian();

std::string serializeString(std::string v);
std::string toHex(char* data, int size);

#endif
