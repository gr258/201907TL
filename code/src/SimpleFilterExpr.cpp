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
    list<string>::iterator it = m_listAttrName.begin();
    list<cJSON*> listParent, listChild;
    listParent.push_back(pRoot);
    int nMatchedNum = 0;

    for(; it != m_listAttrName.end(); it++)
    {
        listChild.clear();
        listChild = GetObject(listParent, (*it).c_str());
        listParent.clear();

        if(0 == listChild.size())
            return false;

        listParent = listChild;
    }
    
    for(list<cJSON*>::iterator jt = listChild.begin(); jt != listChild.end(); jt++)
    {
        cJSON* pAttr = *jt;
        for(it = m_listValue.begin(); it != m_listValue.end(); it++)
        {
            cJSON* pNumber = cJSON_Parse((*it).c_str());
            string strTmp = string("\"") + *it + string("\"");
            cJSON* pString = cJSON_Parse(strTmp.c_str());

            if(cJSON_Compare(pAttr,pNumber,false) || cJSON_Compare(pAttr,pString,false))
            {
                nMatchedNum++;
            }
            else
            {
                Delete(pAttr);
            }
            cJSON_Delete(pNumber);
            cJSON_Delete(pString);
        }
    }

    return nMatchedNum > 0;
}

list<cJSON*> CSimpleFilterExpr::GetObject(cJSON* pParent, const char *strAttrName)
{
    list<cJSON*> ret;

    if(NULL == pParent)
        return ret;

    if(cJSON_IsObject(pParent))
    {
        cJSON *pChild = cJSON_GetObjectItem(pParent, strAttrName);
        if(NULL != pChild)
        {
            m_mapChildParent[pChild] = pParent;
            ret.push_back(pChild);
        }
    }
    else if(cJSON_IsArray(pParent))
    {
        int nSize = cJSON_GetArraySize(pParent);
        for(int i = 0; i < nSize; i++)
        {
            cJSON *pChild = cJSON_GetArrayItem(pParent,i);
            m_mapChildParent[pChild] = pParent;
            list<cJSON*> tmp = GetObject(pChild,strAttrName);
            for(list<cJSON*>::iterator it = tmp.begin(); it != tmp.end(); it++)
            {
                ret.push_back(*it);
            }
            tmp.clear();
        }
    }

    return ret;
}

list<cJSON*> CSimpleFilterExpr::GetObject(list<cJSON*> &listParent, const char *strAttrName)
{
    list<cJSON*> ret;
    for(list<cJSON*>::iterator it = listParent.begin(); it != listParent.end(); it++)
    {
        list<cJSON*> tmp = GetObject(*it,strAttrName);
        for(list<cJSON*>::iterator jt = tmp.begin(); jt != tmp.end(); jt++)
        {
            ret.push_back(*jt);
        }
        tmp.clear();
    }

    return ret;
}

void CSimpleFilterExpr::Delete(cJSON* pNode)
{
    if(NULL == pNode)
        return;

    cJSON* pParent = m_mapChildParent[pNode];
    if(NULL == pParent)
        return;

    cJSON* ppParent = m_mapChildParent[pParent];
    if(!cJSON_IsArray(ppParent))
        return;

    cJSON_Delete(cJSON_DetachItemViaPointer(ppParent, pParent));
}


