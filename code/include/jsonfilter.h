#ifndef JSON_FILTER_H
#define JSON_FILTER_H

#include <string>
#include <list>
#include <map>
#include <malloc.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "cJSON.h"

using namespace std;

#define OP_EQ       0
#define OP_NEQ      1
#define OP_GT       2
#define OP_LT       3
#define OP_GTE      4
#define OP_LTE      5
#define OP_CONT     6
#define OP_NCONT    7

struct CSimpleFilterExpr
{
    list<string>    m_listAttrName;
    list<string>    m_listValue;
    int             m_op;
};

class CJsonFilter
{
public:
    CJsonFilter(const char * jsonfile, const char * filterExpr);
    ~CJsonFilter();
    bool    Parse(string &strJson, string &strFilter);
    void    Clear();
    string  GetResult();
    void    Show(cJSON* pNode);
    string  ReadFile(const char * filename);
    bool    AddFilter(string &strSimpleFilterExpr);
    list<string>    SplitString(string &input_string, char delimiter);
    bool    Match(cJSON* pRoot);
    bool    Match(cJSON* pRoot, CSimpleFilterExpr &sfe);
    list<cJSON*> GetObject(cJSON* pParent, const char *strAttrName);
    list<cJSON*> GetObject(list<cJSON*> &listParent, const char *strAttrName);
    void    Remove(cJSON* pNode);
private:
    cJSON*  m_pJsonData;
    string  m_strResult;
    string  m_strErrMsg;
    list<CSimpleFilterExpr>     m_listFilterExpr;
    map<cJSON*,cJSON*>          m_mapChildParent;
};

#endif
