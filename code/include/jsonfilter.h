#ifndef JSON_FILTER_H
#define JSON_FILTER_H

#include <string>
#include <list>
#include <malloc.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "cJSON.h"

using namespace std;

class CJsonFilter
{
public:
    CJsonFilter(const char * jsonfile, const char * filterExpr);
    ~CJsonFilter();
    bool    Parse(string &strJson, string &strFilter);
    void    Clear();
    string  GetResult();
    void    PrintResult();
    string  ReadFile(const char * filename);
private:
    cJSON*  m_pJsonData;
    string  m_strResult;
    string  m_strErrMsg;
    list<string> m_listSimpleFilterExpr;
};

#endif
