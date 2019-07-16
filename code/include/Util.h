#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <list>

using namespace std;

list<string> SplitString(string &input_string, char delimiter);
string ReadFile(const char * filename);

#endif
