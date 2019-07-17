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

bool CSimpleFilterExpr::ParseValue(string &strValues)
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

bool CSimpleFilterExpr::MatchFilter(cJSON * pRoot)
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
        if(!GetListChildFromParentList(listParent, (*it).c_str(), listAttr))
            return false;

        listParent = listAttr;
    }

    return true;
}

bool CSimpleFilterExpr::GetListChildFromParentList(LJSON &listParent, const char *strAttrName, LJSON &listChild)
{
    listChild.clear();
    for(LJSON::iterator it = listParent.begin(); it != listParent.end(); it++)
    {
        GetListChildFromParentNode(*it, strAttrName, listChild);
    }
    listParent.clear();

    return (listChild.size() > 0);
}

void CSimpleFilterExpr::GetListChildFromParentNode(cJSON* pParent, const char *strAttrName, LJSON &listChild)
{
    if(cJSON_IsObject(pParent))
    {
        GetListChildFromObject(pParent, strAttrName, listChild);
        return;
    }

    if(cJSON_IsArray(pParent))
    {
        GetListChildFromArray(pParent, strAttrName, listChild);
        return;
    }
}

void CSimpleFilterExpr::GetListChildFromObject(cJSON* pParent, const char *strAttrName, LJSON &listChild)
{
    cJSON *pChild = cJSON_GetObjectItem(pParent, strAttrName);
    if(NULL != pChild)
    {
        RecordEntry(pParent, pChild);
        listChild.push_back(pChild);
    }
}

void CSimpleFilterExpr::GetListChildFromArray(cJSON* pParent, const char *strAttrName, LJSON &listChild)
{
    int nSize = cJSON_GetArraySize(pParent);
    for(int i = 0; i < nSize; i++)
    {
        cJSON *pChild = cJSON_GetArrayItem(pParent,i);
        if(NULL != pChild)
        {
            RecordEntry(pParent, pChild);
            GetListChildFromParentNode(pChild, strAttrName, listChild);
        }
    }
}

void CSimpleFilterExpr::RecordEntry(cJSON* pParent, cJSON* pChild)
{
    m_mapChildParent[pChild] = pParent;
}

void CSimpleFilterExpr::DeleteArrayEntry(cJSON* pChild)
{
    if(NULL == pChild)
        return;

    cJSON* pParent = m_mapChildParent[pChild];
    cJSON* ppParent = pParent;
    do
    {
        pParent = ppParent;
        ppParent = m_mapChildParent[pParent];
    }while(cJSON_IsObject(ppParent));

    if(!cJSON_IsArray(ppParent))
        return;

    cJSON_Delete(cJSON_DetachItemViaPointer(ppParent, pParent));
}

bool CSimpleFilterExpr::IsAttrListMatchValues(LJSON &listAttr)
{
    int nMatchedNum = 0;
    
    for(LJSON::iterator it = listAttr.begin(); it != listAttr.end(); it++)
    {
        if(IsAttrNodeMatchValues(*it))
            nMatchedNum++;
        else
            DeleteArrayEntry(*it);
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

bool CSimpleFilterExpr::CompareString(cJSON* pAttr, string &strValue)
{
    string strAttr = pAttr->valuestring;
    double diff = strAttr.compare(strValue);
    return CompareResult(diff);
}

bool CSimpleFilterExpr::CompareNumber(cJSON* pAttr, string &strValue)
{
    double  diff = 0;
    bool    bValidNumber = false;
    cJSON*  pValue = cJSON_Parse(strValue.c_str());
    if(cJSON_IsNumber(pValue))
    {
        diff = pAttr->valuedouble - pValue->valuedouble;
        bValidNumber = true;
    }
    cJSON_Delete(pValue);
    
    return bValidNumber?CompareResult(diff):false;
}

bool CSimpleFilterExpr::CompareArray(cJSON* pAttr, string &strValue)
{
    int nMatchedNum = 0, nSize = cJSON_GetArraySize(pAttr);

    if(nSize < 0)
        return false;

    for(int i = 0; i < nSize; i++)
    {
        cJSON *pChild = cJSON_GetArrayItem(pAttr,i);
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


