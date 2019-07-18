#include "JsonFilter.h"
#include "Util.h"

CJsonFilter::CJsonFilter(const char * jsonfile, const char * filterExpr)
    : m_pJsonData(NULL)
{
    m_strResult = "[]";
    string strJson = ReadFile(jsonfile);
    string strFilter(filterExpr);
    if(ParseInput(strJson,strFilter))
    {
        FilterOutMatchList();
        SaveResult();
    }
}

CJsonFilter::~CJsonFilter()
{
    Clear();
}

string CJsonFilter::GetResult()
{
    return m_strResult;
}

void CJsonFilter::Clear()
{
    if (m_pJsonData != NULL)
    {
        cJSON_Delete(m_pJsonData);
        m_pJsonData = NULL;
    }
    m_listFilterExpr.clear();
}

bool CJsonFilter::ParseInput(string &strJson, string &strFilters)
{
    if(!ParseJsonData(strJson))
        return false;

    if(!ParseFilters(strFilters))
        return false;

    return true;
}

bool CJsonFilter::ParseJsonData(string &strJson)
{
    m_pJsonData = cJSON_Parse(strJson.c_str());
    if (m_pJsonData == NULL)
        return false;

    return true;
}

bool CJsonFilter::ParseFilters(string &strFilters)
{
    LSTR listFilter = SplitString(strFilters, '&');
    if(0 == listFilter.size())
        return false;

    for(LSTR::iterator it = listFilter.begin(); it != listFilter.end(); it++)
    {
        if(!ParseFilter(*it))
        {
            return false;
        }
    }
    
    return true;
}

bool CJsonFilter::ParseFilter(string &strFilter)
{
    CSimpleFilterExpr sfe;

    if(!sfe.ParseFilter(strFilter))
        return false;

    m_listFilterExpr.push_back(sfe);
    return true;
}

void CJsonFilter::FilterOutMatchList()
{
    if(!cJSON_IsArray(m_pJsonData))
        return;

    LJSON listUnmatch;
    GetUnmatchList(listUnmatch);
    DelUnmatchList(listUnmatch);
    SaveResult();
}

void CJsonFilter::GetUnmatchList(LJSON &listUnmatch)
{
    cJSON *pChild = NULL;
    cJSON_ArrayForEach(pChild,m_pJsonData)
    {
        if(!IsMatchFilter(pChild))
            listUnmatch.push_back(pChild);
    }
}

void CJsonFilter::DelUnmatchList(LJSON &listUnmatch)
{
    for(LJSON::iterator it = listUnmatch.begin(); it != listUnmatch.end(); it++)
    {
        cJSON_Delete(cJSON_DetachItemViaPointer(m_pJsonData, *it));
    }
}

void CJsonFilter::SaveResult()
{
    char* pJsonString = cJSON_PrintUnformatted(m_pJsonData);
    if (pJsonString != NULL)
    {
        m_strResult = pJsonString;
        cJSON_free(pJsonString);
    }
}

bool CJsonFilter::IsMatchFilter(cJSON* pRoot)
{
    bool bRet = true;

    //CSimpleFilterExpr.IsMatchFilter() function will delete unmatch entry, so duplicate
    cJSON* pDup = cJSON_Duplicate(pRoot, true);
    for(LSFE::iterator it = m_listFilterExpr.begin(); it != m_listFilterExpr.end(); it++)
    {
        if(!(*it).IsMatchFilter(pDup))
        {
            bRet = false;
            break;
        }
    }

    cJSON_Delete(pDup);
    return bRet;
}

