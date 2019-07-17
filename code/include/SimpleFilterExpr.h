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
    bool    MatchFilter(cJSON* pRoot);

private:    
    bool    ParseAttrsAndOp(string &strAttrsAndOp);
    bool    ParseValue(string &strValues);
    void    ParseOp();
    bool    IsOneValueOp();
    bool    IsNotOp();
    bool    IsArrayOp();
    bool    GetAttrList(cJSON*       pRoot, LJSON &listAttr);
    bool    GetListChildFromParentList(LJSON &listParent, const char *strAttrName, LJSON &listChild);
    void    GetListChildFromParentNode(cJSON* pParent, const char *strAttrName, LJSON &listChild);
    void    GetListChildFromObject(cJSON* pParent, const char *strAttrName, LJSON &listChild);
    void    GetListChildFromArray(cJSON* pParent, const char *strAttrName, LJSON &listChild);
    void    RecordEntry(cJSON* pParent, cJSON* pChild);
    void    DeleteArrayEntry(cJSON* pChild);
    bool    IsAttrListMatchValues(LJSON &listAttr);
    bool    IsAttrNodeMatchValues(cJSON* pAttr);
    bool    IsAttrNodeMatchOp(cJSON* pAttr);
    bool    CompareValue(cJSON* pAttr, string &strValue);
    bool    CompareString(cJSON* pAttr, string &strValue);
    bool    CompareNumber(cJSON* pAttr, string &strValue);
    bool    CompareArray(cJSON* pAttr, string &strValue);
    bool    CompareResult(double diff);

private:
    LSTR    m_listAttrName;
    LSTR    m_listValue;
    int     m_nOp;
    MJSON   m_mapChildParent;
};

#endif


