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
    void    Delete(cJSON* pNode);
    list<cJSON*>    GetObject(cJSON* pParent, const char *strAttrName);
    list<cJSON*>    GetObject(list<cJSON*> &listParent, const char *strAttrName);

private:
    list<string>        m_listAttrName;
    list<string>        m_listValue;
    int                 m_nOp;
    map<string,int>     m_mapOp;
    map<cJSON*,cJSON*>  m_mapChildParent;
};

#endif


