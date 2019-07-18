#ifndef JSON_FILTER_H
#define JSON_FILTER_H

#include <string>
#include <list>
#include <map>
#include "cJSON.h"
#include "SimpleFilterExpr.h"

using namespace std;

typedef list<CSimpleFilterExpr> LSFE;

class CJsonFilter
{
public:
    CJsonFilter(const char * jsonfile, const char * filterExpr);
    ~CJsonFilter();
    string  GetResult();

private:    
    void    Clear();
    
    bool    ParseInput(string &strJson, string &strFilters);
    bool    ParseJsonData(string &strJson);
    bool    ParseFilters(string &strFilters);
    bool    ParseFilter(string &strFilter);
    
    void    FilterOutMatchList();
    void    GetUnmatchList(LJSON &listUnmatch);
    void    DelUnmatchList(LJSON &listUnmatch);
    bool    IsMatchFilter(cJSON* pRoot);
    void    SaveResult();

private:
    cJSON*  m_pJsonData;
    string  m_strResult;
    LSFE    m_listFilterExpr;
};

#endif

