#ifndef JSON_FILTER_H
#define JSON_FILTER_H

#include <string>
#include <list>
#include <map>
#include "cJSON.h"
#include "SimpleFilterExpr.h"

using namespace std;

class CJsonFilter
{
public:
    CJsonFilter(const char * jsonfile, const char * filterExpr);
    ~CJsonFilter();
    string  GetResult();

private:    
    void    Clear();
    bool    Parse(string &strJson, string &strFilters);
    bool    ParseJsonData(string &strJson);
    bool    ParseFilters(string &strFilters);
    bool    ParseFilter(string &strFilter);
    void    Filter();
    bool    MatchFilter(cJSON* pRoot);

private:
    cJSON*  m_pJsonData;
    string  m_strResult;
    list<CSimpleFilterExpr>     m_listFilterExpr;
};

#endif

