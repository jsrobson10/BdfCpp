
#ifndef HELPERS_H_
#define HELPERS_H_

#include <string>

void reverseIfLittleEndian(void* to, const void* from, int size);
void reverseArray(void* to, const void* from, int size);
bool isLittleEndian();

std::string serializeString(std::string v);
std::string toHex(char* data, int size);

#endif
