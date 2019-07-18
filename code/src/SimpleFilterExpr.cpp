#include "SimpleFilterExpr.h"
#include "Util.h"

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
    
    LSTR strList = SplitString(strFilter, '=');
    if(strList.size() != 2)
        return false;

    if(!ParseAttrsAndOp(strList.front()))
        return false;
    
    if(!ParseValues(strList.back()))
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

bool CSimpleFilterExpr::ParseValues(string &strValues)
{
    m_listValue = SplitString(strValues, ',');
    if(m_listValue.size() <= 0)
        return false;

    if(IsOneValueOp() && m_listValue.size() > 1)
    {
        return false;
    }

    return true;
}

bool CSimpleFilterExpr::IsOneValueOp()
{
    if(OP_GT == m_nOp || OP_LT == m_nOp || OP_GTE == m_nOp || OP_LTE == m_nOp)
        return true;

    return false;
}

bool CSimpleFilterExpr::IsNotOp()
{
    if(OP_NEQ == m_nOp || OP_NCONT == m_nOp)
        return true;

    return false;
}

bool CSimpleFilterExpr::IsArrayOp()
{
    if(OP_CONT == m_nOp || OP_NCONT == m_nOp)
        return true;

    return false;
}

bool CSimpleFilterExpr::IsMatchFilter(cJSON * pRoot)
{
    LJSON listAttr;

    if(!GetAttrList(pRoot, listAttr))
        return false;

    return IsAttrListMatchValues(listAttr);
}

bool CSimpleFilterExpr::GetAttrList(cJSON*       pRoot, LJSON &listAttr)
{
    LJSON listParent;
    listParent.push_back(pRoot);

    for(LSTR::iterator it = m_listAttrName.begin(); it != m_listAttrName.end(); it++)
    {
        if(!GetChildListFromParentList(listParent, (*it).c_str(), listAttr))
            return false;

        listParent = listAttr;
    }

    return true;
}

bool CSimpleFilterExpr::GetChildListFromParentList(LJSON &listParent, const char *strAttrName, LJSON &listChild)
{
    listChild.clear();
    for(LJSON::iterator it = listParent.begin(); it != listParent.end(); it++)
    {
        GetChildListFromParentNode(*it, strAttrName, listChild);
    }
    listParent.clear();

    return (listChild.size() > 0);
}

void CSimpleFilterExpr::GetChildListFromParentNode(cJSON* pParent, const char *strAttrName, LJSON &listChild)
{
    if(cJSON_IsObject(pParent))
    {
        GetChildListFromObjectNode(pParent, strAttrName, listChild);
        return;
    }

    if(cJSON_IsArray(pParent))
    {
        GetChildListFromArrayNode(pParent, strAttrName, listChild);
        return;
    }
}

void CSimpleFilterExpr::GetChildListFromObjectNode(cJSON* pParent, const char *strAttrName, LJSON &listChild)
{
    cJSON *pChild = cJSON_GetObjectItem(pParent, strAttrName);
    if(NULL != pChild)
    {
        SetParent(pChild, pParent);
        listChild.push_back(pChild);
    }
}

void CSimpleFilterExpr::GetChildListFromArrayNode(cJSON* pParent, const char *strAttrName, LJSON &listChild)
{
    cJSON *pChild = NULL;
    cJSON_ArrayForEach(pChild,pParent)
    {
        SetParent(pChild, pParent);
        GetChildListFromParentNode(pChild, strAttrName, listChild);
    }
}

void CSimpleFilterExpr::SetParent(cJSON* pChild, cJSON* pParent)
{
    m_mapChildParent[pChild] = pParent;
}

cJSON* CSimpleFilterExpr::GetParent(cJSON* pChild)
{
    if(NULL == pChild)
        return NULL;

    if(0 == m_mapChildParent.count(pChild))
        return NULL;

    return m_mapChildParent[pChild];
}

void CSimpleFilterExpr::DelParentEntry(cJSON* pChild)
{
    cJSON* pParent = GetParent(pChild);
    cJSON* ppParent = pParent;
    do
    {
        pParent = ppParent;
        ppParent = GetParent(pParent);
    }while(cJSON_IsObject(ppParent));

    if(!cJSON_IsArray(ppParent))
        return;

    cJSON_Delete(cJSON_DetachItemViaPointer(ppParent, pParent));
    
    if(0 == cJSON_GetArraySize(ppParent))
        DelParentEntry(ppParent);
}

bool CSimpleFilterExpr::IsAttrListMatchValues(LJSON &listAttr)
{
    int nMatchedNum = 0;
    
    for(LJSON::iterator it = listAttr.begin(); it != listAttr.end(); it++)
    {
        if(IsAttrNodeMatchValues(*it))
            nMatchedNum++;
        else
            DelParentEntry(*it);
    }
    
    return (nMatchedNum > 0);
}

bool CSimpleFilterExpr::IsAttrNodeMatchValues(cJSON* pAttr)
{
    int nMatchedNum = 0, nSize = m_listValue.size();
    
    for(LSTR::iterator it = m_listValue.begin(); it != m_listValue.end(); it++)
    {
        if(!IsAttrNodeMatchOp(pAttr))
            continue;

        if(CompareValue(pAttr, *it))
        {
            nMatchedNum++;
        }
    }

    if(IsNotOp())
        return (nMatchedNum == nSize);
    else
        return (nMatchedNum > 0);
}

bool CSimpleFilterExpr::IsAttrNodeMatchOp(cJSON* pAttr)
{
    return (!IsArrayOp() == !cJSON_IsArray(pAttr));
}

bool CSimpleFilterExpr::CompareValue(cJSON* pAttr, string &strValue)
{
    if(cJSON_IsString(pAttr))
    {
        return CompareString(pAttr, strValue);
    }

    if(cJSON_IsNumber(pAttr))
    {
        return CompareNumber(pAttr, strValue);
    }

    if(cJSON_IsArray(pAttr))
    {
        return CompareArray(pAttr, strValue);
    }

    return false;
}

bool CSimpleFilterExpr::CompareString(cJSON* pString, string &strValue)
{
    string strAttr = pString->valuestring;
    double diff = strAttr.compare(strValue);
    return CompareResult(diff);
}

bool CSimpleFilterExpr::CompareNumber(cJSON* pNumber, string &strValue)
{
    double  diff = 0;
    bool    bValidNumber = false;
    cJSON*  pValue = cJSON_Parse(strValue.c_str());
    if(cJSON_IsNumber(pValue))
    {
        diff = pNumber->valuedouble - pValue->valuedouble;
        bValidNumber = true;
    }
    cJSON_Delete(pValue);
    
    return bValidNumber?CompareResult(diff):false;
}

bool CSimpleFilterExpr::CompareArray(cJSON* pArray, string &strValue)
{
    int nMatchedNum = 0, nSize = cJSON_GetArraySize(pArray);

    if(0 == nSize)
        return false;

    cJSON *pChild = NULL;
    cJSON_ArrayForEach(pChild,pArray)
    {
        if(CompareValue(pChild, strValue))
            nMatchedNum++;
    }

    if(IsNotOp())
        return (nMatchedNum == nSize);
    else
        return (nMatchedNum > 0);
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


