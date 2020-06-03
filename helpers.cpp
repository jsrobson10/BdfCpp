
#include "helpers.h"
#include <string.h>
#include <iostream>

char hex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

char* reverseArray(void* array, int size)
{
  char* reversed = new char[size];
  char* array2 = (char*)array;

  for(int i=0;i<size;i++) {
    reversed[size - i - 1] = array2[i];
  }

  return reversed;
}

bool isLittleEndian() {
  short* check = new short[1] {0x01};
  bool r = ((char*)(check))[0] == 0x01;
  delete[] check;
  return r;
}

char* reverseIfLittleEndian(void* array, int size)
{
  char* reversed;

  if(isLittleEndian()) {
    reversed = reverseArray(array, size);
  } else {
    reversed = new char[size];
    memcpy(reversed, array, size);
  }

  return reversed;
}

std::string serializeString(std::string str)
{
  std::string modified = "";

  for(int i=0;i<str.size();i++)
  {
    char c = str[i];

    if(c == '\\') modified += "\\\\";
    else if(c == '\n') modified += "\\n";
    else if(c == '\t') modified += "\\t";
    else if(c == '"') modified += "\\\"";

    else if(!((c >= 0x21 && c <= 0x7E) || c >= 0xA1)) {
      modified += "\\b";
      modified += hex[c / 16];
      modified += hex[c % 16];
    }

    else {
      modified += str[i];
    }
  }

  return "\"" + modified + "\"";
}

std::string toHex(char* data, int size)
{
  std::string r;

  for(int i=0;i<size;i++) {
    char c = data[i];
    r += hex[c / 16];
    r += hex[c % 16];
    r += " ";
  }

  return r;
}
