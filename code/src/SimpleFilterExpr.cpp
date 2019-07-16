#include "SimpleFilterExpr.h"
#include "Util.h"

CSimpleFilterExpr::CSimpleFilterExpr()
{
    m_mapOp["eq"]       =   OP_EQ;
    m_mapOp["neq"]      =   OP_NEQ;
    m_mapOp["gt"]       =   OP_GT;
    m_mapOp["lt"]       =   OP_LT;
    m_mapOp["gte"]      =   OP_GTE;
    m_mapOp["lte"]      =   OP_LTE;
    m_mapOp["cont"]     =   OP_CONT;
    m_mapOp["ncont"]    =   OP_NCONT;
}

CSimpleFilterExpr::~CSimpleFilterExpr()
{
    Clear();
}

void CSimpleFilterExpr::Clear()
{
    m_listAttrName.clear();
    m_listValue.clear();
    m_mapOp.clear();
    m_mapChildParent.clear();
}

bool CSimpleFilterExpr::ParseFilter(string &strFilter)
{
    Clear();
    
    list<string> strList = SplitString(strFilter, '=');
    if(strList.size() != 2)
        return false;

    if(!ParseAttrsAndOp(strList.front()))
        return false;
    
    if(!ParseValue(strList.back()))
        return false;
    
    return true;
}

bool CSimpleFilterExpr::ParseAttrsAndOp(string &strAttrsAndOp)
{
    m_listAttrName = SplitString(strAttrsAndOp, '.');
    if(m_listAttrName.size() <= 0)
        return false;

    ParseOp();

    return true;
}

bool CSimpleFilterExpr::ParseValue(string &strValues)
{
    m_listValue = SplitString(strValues, '.');
    if(m_listValue.size() <= 0)
        return false;

    return true;
}

void CSimpleFilterExpr::ParseOp()
{
    m_nOp = m_mapOp["eq"];
    
    if(m_listAttrName.size() <= 1)
        return;
    
    string strOp = m_listAttrName.back();
    if(0 == m_mapOp.count(strOp))
        return;
        
    m_nOp = m_mapOp[strOp];
    m_listAttrName.pop_back();
}

bool CSimpleFilterExpr::MatchFilter(cJSON * pRoot)
{
    list<cJSON*> listAttrName;

    if(!GetAttrsList(pRoot, listAttrName))
        return false;

    return MatchValue(listAttrName);
}

bool CSimpleFilterExpr::GetAttrsList(cJSON*       pRoot, list<cJSON*> &listAttr)
{
    list<cJSON*> listParent;
    listParent.push_back(pRoot);

    for(list<string>::iterator it = m_listAttrName.begin(); it != m_listAttrName.end(); it++)
    {
        if(!GetObject(listParent, (*it).c_str(), listAttr))
            return false;

        listParent = listAttr;
    }

    return true;
}

bool CSimpleFilterExpr::GetObject(list<cJSON*> &listParent, const char *strAttrName, list<cJSON*> &listChild)
{
    listChild.clear();
    for(list<cJSON*>::iterator it = listParent.begin(); it != listParent.end(); it++)
    {
        GetObject(*it, strAttrName, listChild);
    }
    listParent.clear();

    return (listChild.size() > 0);
}

void CSimpleFilterExpr::GetObject(cJSON* pParent, const char *strAttrName, list<cJSON*> &listChild)
{
    if(cJSON_IsObject(pParent))
    {
        cJSON *pChild = cJSON_GetObjectItem(pParent, strAttrName);
        if(NULL != pChild)
        {
            m_mapChildParent[pChild] = pParent;
            listChild.push_back(pChild);
        }
    }
    else if(cJSON_IsArray(pParent))
    {
        int nSize = cJSON_GetArraySize(pParent);
        for(int i = 0; i < nSize; i++)
        {
            cJSON *pChild = cJSON_GetArrayItem(pParent,i);
            m_mapChildParent[pChild] = pParent;
            GetObject(pChild, strAttrName, listChild);
        }
    }
}

void CSimpleFilterExpr::DeleteEntry(cJSON* pChild)
{
    if(NULL == pChild)
        return;

    cJSON* pParent = m_mapChildParent[pChild];
    if(NULL == pParent)
        return;

    cJSON* pEntry = m_mapChildParent[pParent];
    if(!cJSON_IsArray(pEntry))
        return;

    cJSON_Delete(cJSON_DetachItemViaPointer(pEntry, pParent));
}

bool CSimpleFilterExpr::MatchValue(list<cJSON*> &listAttr)
{
    int nMatchedNum = 0;
    
    for(list<cJSON*>::iterator it = listAttr.begin(); it != listAttr.end(); it++)
    {
        if(MatchValue(*it))
            nMatchedNum++;
    }
    
    return (nMatchedNum > 0);
}

bool CSimpleFilterExpr::MatchValue(cJSON* pAttr)
{
    int nMatchedNum = 0;
    
    for(list<string>::iterator it = m_listValue.begin(); it != m_listValue.end(); it++)
    {
        if(MatchValue(pAttr, *it))
        {
            nMatchedNum++;
        }
        else
        {
            DeleteEntry(pAttr);
        }
    }

    return (nMatchedNum > 0);
}

bool CSimpleFilterExpr::MatchValue(cJSON* pAttr, string &strValue)
{
    bool bResult = false;
    
    if(cJSON_IsString(pAttr))
    {
        string strAttr = pAttr->valuestring;
        bResult = MatchStringValue(strAttr, strValue);
    }
    else if(cJSON_IsNumber(pAttr))
    {
        cJSON* pValue = cJSON_Parse(strValue.c_str());
        if(cJSON_IsNumber(pValue))
            bResult = MatchNumberValue(pAttr->valuedouble, pValue->valuedouble);

        cJSON_Delete(pValue);
    }

    return bResult;
}

bool CSimpleFilterExpr::MatchStringValue(string &left, string &right)
{
    switch(m_nOp)
    {
        case OP_EQ:
            return (left.compare(right) == 0);
        default:
            break;
    }
    return false;
}

bool CSimpleFilterExpr::MatchNumberValue(double left, double right)
{
    switch(m_nOp)
    {
        case OP_EQ:
            return (left == right);
        default:
            break;
    }
    return false;
}


