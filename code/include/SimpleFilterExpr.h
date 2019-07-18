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

typedef list<string>        LSTR;
typedef list<cJSON*>        LJSON;
typedef map<cJSON*,cJSON*>  MJSON;

class CSimpleFilterExpr
{
public:
    ~CSimpleFilterExpr();
    void    Clear();
    bool    ParseFilter(string &strFilter);
    bool    IsMatchFilter(cJSON* pRoot);

private:    
    bool    ParseAttrsAndOp(string &strAttrsAndOp);
    bool    ParseValues(string &strValues);
    void    ParseOp();
    
    bool    IsOneValueOp();
    bool    IsNotOp();
    bool    IsArrayOp();
    
    bool    GetAttrList(cJSON*       pRoot, LJSON &listAttr);
    bool    GetChildListFromParentList(LJSON &listParent, const char *strAttrName, LJSON &listChild);
    void    GetChildListFromParentNode(cJSON* pParent, const char *strAttrName, LJSON &listChild);
    void    GetChildListFromObjectNode(cJSON* pParent, const char *strAttrName, LJSON &listChild);
    void    GetChildListFromArrayNode(cJSON* pParent, const char *strAttrName, LJSON &listChild);

    void    SetParent(cJSON* pChild, cJSON* pParent);
    cJSON*  GetParent(cJSON* pChild);
    void    DelParentEntry(cJSON* pChild);

    bool    IsAttrListMatchValues(LJSON &listAttr);
    bool    IsAttrNodeMatchValues(cJSON* pAttr);
    bool    IsAttrNodeMatchOp(cJSON* pAttr);

    bool    CompareValue(cJSON* pAttr, string &strValue);
    bool    CompareString(cJSON* pString, string &strValue);
    bool    CompareNumber(cJSON* pNumber, string &strValue);
    bool    CompareArray(cJSON* pArray, string &strValue);
    bool    CompareResult(double diff);

private:
    int     m_nOp;
    LSTR    m_listAttrName;
    LSTR    m_listValue;
    MJSON   m_mapChildParent;
};

#endif


