#include "Util.h"
#include <fstream>
#include <sstream>
#include <iostream>

list<string> SplitString(string &input_string, char delimiter)
{
    size_t start = 0, end = 0, nSize = input_string.size();
    list<string> splits;

    if(input_string[0] == delimiter || input_string[nSize - 1] == delimiter)
    {
        return splits;
    }

    size_t i = 0;
    size_t pos = input_string.find(delimiter);

    while (pos != string::npos) {
        splits.push_back(input_string.substr(i, pos - i));

        i = pos + 1;
        pos = input_string.find(delimiter, i);
    }

    splits.push_back(input_string.substr(i, min(pos, nSize - i)));

    return splits;
}

string ReadFile(const char * filename)
{
    ifstream ifile(filename);
    ostringstream buf;
    char ch;

    while(buf&&ifile.get(ch))
    {
        buf.put(ch);
    }

    return buf.str();
}

