#ifndef SIMPLE_FILTER_EXPR_H
#define SIMPLE_FILTER_EXPR_H

#include <string>
#include <list>
#include <map>
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

class CSimpleFilterExpr
{
public:
    CSimpleFilterExpr();
    ~CSimpleFilterExpr();
    void    Clear();
    bool    ParseFilter(string &strFilter);
    bool    MatchFilter(cJSON* pRoot);

private:    
    bool    ParseAttrsAndOp(string &strAttrsAndOp);
    bool    ParseValue(string &strValues);
    void    ParseOp();
    bool    GetAttrsList(cJSON*       pRoot, list<cJSON*> &listAttr);
    bool    GetObject(list<cJSON*> &listParent, const char *strAttrName, list<cJSON*> &listChild);
    void    GetObject(cJSON* pParent, const char *strAttrName, list<cJSON*> &listChild);
    void    DeleteEntry(cJSON* pChild);
    bool    MatchValue(list<cJSON*> &listAttr);
    bool    MatchValue(cJSON* pAttr);
    bool    MatchValue(cJSON* pAttr, string &strValue);
    bool    CompareString(string &strAttr, string &strValue);
    bool    CompareNumber(double dAttr, double dValue);
    bool    CompareResult(double diff);

private:
    list<string>        m_listAttrName;
    list<string>        m_listValue;
    int                 m_nOp;
    map<cJSON*,cJSON*>  m_mapChildParent;
};

#endif


