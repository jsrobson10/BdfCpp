
#ifndef HELPERS_H_
#define HELPERS_H_

#include <string>

void reverseIfLittleEndian(void* to, const void* from, int size);

std::string serializeString(std::string v);

#endif
