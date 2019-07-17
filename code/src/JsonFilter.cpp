#include "JsonFilter.h"
#include "Util.h"
#include <malloc.h>

CJsonFilter::CJsonFilter(const char * jsonfile, const char * filterExpr)
    : m_pJsonData(NULL)
{
    string strJson = ReadFile(jsonfile);
    string strFilter(filterExpr);
    Parse(strJson,strFilter);
    Filter();
}

CJsonFilter::~CJsonFilter()
{
    Clear();
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

bool CJsonFilter::Parse(string &strJson, string &strFilters)
{
    Clear();

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
    list<string> listFilter = SplitString(strFilters, '&');
    if(0 == listFilter.size())
        return false;

    for(list<string>::iterator it = listFilter.begin(); it != listFilter.end(); it++)
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

void CJsonFilter::Filter()
{
    m_strResult = "[]";
    if(!cJSON_IsArray(m_pJsonData))
        return;
    
    int nSize = cJSON_GetArraySize(m_pJsonData);
    list<cJSON*> toDelete;
    for(int i = 0; i < nSize; i++)
    {
        cJSON* pArryItem = cJSON_GetArrayItem(m_pJsonData, i);
        if(!MatchFilter(pArryItem))
            toDelete.push_back(pArryItem);
    }

    for(list<cJSON*>::iterator it = toDelete.begin(); it != toDelete.end(); it++)
    {
        cJSON_Delete(cJSON_DetachItemViaPointer(m_pJsonData, *it));
    }
    
    char* pJsonString = cJSON_PrintUnformatted(m_pJsonData);
    if (pJsonString != NULL)
    {
        m_strResult = pJsonString;
        cJSON_free(pJsonString);
    }
}

string CJsonFilter::GetResult()
{
    return m_strResult;
}

bool CJsonFilter::MatchFilter(cJSON* pRoot)
{
    bool bRet = true;
    
    if(0 == m_listFilterExpr.size())
        return false;

    cJSON* pDup = cJSON_Duplicate(pRoot, true);
    for(list<CSimpleFilterExpr>::iterator it = m_listFilterExpr.begin(); it != m_listFilterExpr.end(); it++)
    {
        if(!(*it).MatchFilter(pDup))
        {
            bRet = false;
            break;
        }
    }

    cJSON_Delete(pDup);
    return bRet;
}

