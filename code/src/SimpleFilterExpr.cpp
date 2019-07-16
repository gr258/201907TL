#include "SimpleFilterExpr.h"
#include "Util.h"

CSimpleFilterExpr::CSimpleFilterExpr()
{

}

CSimpleFilterExpr::~CSimpleFilterExpr()
{
    Clear();
}

void CSimpleFilterExpr::Clear()
{
    m_listAttrName.clear();
    m_listValue.clear();
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
    m_listValue = SplitString(strValues, ',');
    if(m_listValue.size() <= 0)
        return false;

    return true;
}

void CSimpleFilterExpr::ParseOp()
{
    m_nOp = OP_EQ;
    
    if(m_listAttrName.size() <= 1)
        return;
    
    string strOp = m_listAttrName.back();
    map<string, int> mapOp;
    mapOp["eq"] = OP_EQ;
    mapOp["neq"] = OP_NEQ;
    mapOp["gt"] = OP_GT;
    mapOp["lt"] = OP_LT;
    mapOp["gte"] = OP_GTE;
    mapOp["lte"] = OP_LTE;
    mapOp["cont"] = OP_CONT;
    mapOp["ncont"] = OP_NCONT;

    if(0 == mapOp.count(strOp))
        return;

    m_nOp = mapOp[strOp];
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
    }

    if(0 == nMatchedNum)
    {
        DeleteEntry(pAttr);
        return false;
    }
    
    return true;
}

bool CSimpleFilterExpr::MatchValue(cJSON* pAttr, string &strValue)
{
    bool bResult = false;
    
    if(cJSON_IsString(pAttr))
    {
        string strAttr = pAttr->valuestring;
        bResult = CompareString(strAttr, strValue);
    }
    else if(cJSON_IsNumber(pAttr))
    {
        cJSON* pValue = cJSON_Parse(strValue.c_str());
        if(cJSON_IsNumber(pValue))
            bResult = CompareNumber(pAttr->valuedouble, pValue->valuedouble);

        cJSON_Delete(pValue);
    }
    else if(cJSON_IsArray(pAttr))
    {
    }

    return bResult;
}

bool CSimpleFilterExpr::CompareString(string &strAttr, string &strValue)
{
    double diff = strValue.compare(strAttr);
    return CompareResult(diff);
}

bool CSimpleFilterExpr::CompareNumber(double dAttr, double dValue)
{
    double diff = dAttr - dValue;
    return CompareResult(diff);
}

bool CSimpleFilterExpr::CompareResult(double diff)
{
    switch(m_nOp)
    {
        case OP_EQ:
        case OP_CONT:
            return (diff == 0);
        case OP_NEQ:
        case OP_NCONT:
            return (diff != 0);
        case OP_GT:
            return (diff > 0);
        case OP_LT:
            return (diff < 0);
        case OP_GTE:
            return (diff >= 0);
        case OP_LTE:
            return (diff <= 0);
        default:
            break;
    }
    return false;
}


