#include "jsonfilter.h"

string JsonFilter(const char * jsonfile, const char * filterExpr)
{
    CJsonFilter jf(jsonfile,filterExpr);

    return jf.GetResult();
}

CJsonFilter::CJsonFilter(const char * jsonfile, const char * filterExpr)
    : m_pJsonData(NULL)
{
    string strJson = ReadFile(jsonfile);
    string strFilter(filterExpr);
    Parse(strJson,strFilter);
}

CJsonFilter::~CJsonFilter()
{
    Clear();
}

bool CJsonFilter::Parse(string &strJson, string &strFilterExpr)
{
    m_pJsonData = cJSON_Parse(strJson.c_str());
    if (m_pJsonData == NULL)
    {
        m_strErrMsg = string("cJSON_Parse error at ") + cJSON_GetErrorPtr();
        return false;
    }
    
    list<string> strlist = SplitString(strFilterExpr, '&');
    if(0 == strlist.size())
    {
        m_strErrMsg = string("Parse error 1");
        return false;
    }

    for(list<string>::iterator it = strlist.begin(); it != strlist.end(); it++)
    {
        if(!AddFilter(*it))
        {
            m_strErrMsg = string("Parse error 2");
            return false;
        }
    }

    return true;
}

void CJsonFilter::Clear()
{
    if (m_pJsonData != NULL)
    {
        cJSON_Delete(m_pJsonData);
        m_pJsonData = NULL;
    }
}

string CJsonFilter::GetResult()
{
    m_strResult = "[\n";
    if (cJSON_IsArray(m_pJsonData))
    {
        int nSize = cJSON_GetArraySize(m_pJsonData);
        for(int i = 0; i < nSize; i++)
        {
            cJSON* pJsonStruct = cJSON_GetArrayItem(m_pJsonData, i);
            cJSON* pDup = cJSON_Duplicate(pJsonStruct, true);
            if(Match(pDup))
            {
                char* pJsonString = cJSON_PrintUnformatted(pJsonStruct);
                if (pJsonString != NULL)
                {
                    m_strResult += pJsonString + string("\n");
                    free(pJsonString);
                }
            }
            cJSON_Delete(pDup);
        }
    }
    m_strResult += "]";
    
    return m_strResult;
}

void CJsonFilter::Show(cJSON* pNode)
{
    char* pJsonString = cJSON_PrintUnformatted(pNode);
    if (pJsonString != NULL)
    {
        printf("%s\n", pJsonString);
        free(pJsonString);
    }
}

string CJsonFilter::ReadFile(const char * filename)
{
    ifstream ifile(filename);
    ostringstream buf;
    char ch;

    while(buf&&ifile.get(ch))
    {
        buf.put(ch);
    }

    return buf.str();
}

bool CJsonFilter::AddFilter(string &strSimpleFilterExpr)
{
    size_t pos = 0, nSize = strSimpleFilterExpr.size();
    CSimpleFilterExpr sfe;

    pos = strSimpleFilterExpr.find('=', 0);
    if(pos <= 0 || pos >= nSize -1)
    {
        m_strErrMsg = string("AddFilter error 1");
        return false;
    }

    string strTmp = strSimpleFilterExpr.substr(0, pos);
    sfe.m_listAttrName = SplitString(strTmp, '.');

    if(sfe.m_listAttrName.size() > 1 && sfe.m_listAttrName.back() == "eq")
    {
        sfe.m_listAttrName.pop_back();
    }

    strTmp = strSimpleFilterExpr.substr(pos + 1, nSize - pos);
    sfe.m_listValue = SplitString(strTmp, '.');

    m_listFilterExpr.push_back(sfe);

    return true;
}

list<string> CJsonFilter::SplitString(string &input_string, char delimiter)
{
    size_t start = 0, end = 0, nSize = input_string.size();
    list<string> splits;

    if(input_string[0] == delimiter || input_string[nSize - 1] == delimiter)
    {
        return splits;
    }

    size_t i = 0;
    size_t pos = input_string.find(delimiter);

    while (pos != string::npos) {
        splits.push_back(input_string.substr(i, pos - i));

        i = pos + 1;
        pos = input_string.find(delimiter, i);
    }

    splits.push_back(input_string.substr(i, min(pos, nSize - i)));

    return splits;
}

bool CJsonFilter::Match(cJSON* pRoot)
{
    if(0 == m_listFilterExpr.size())
    {
        return false;
    }

    for(list<CSimpleFilterExpr>::iterator it = m_listFilterExpr.begin(); it != m_listFilterExpr.end(); it++)
    {
        if(!Match(pRoot,*it))
        {
            return false;
        }
    }

    Show(pRoot);

    return true;
}

bool CJsonFilter::Match(cJSON* pRoot, CSimpleFilterExpr &sfe)
{
    list<string>::iterator it = sfe.m_listAttrName.begin();
    list<cJSON*> listParent, listChild;
    listParent.push_back(pRoot);
    int nMatchedNum = 0;

    for(; it != sfe.m_listAttrName.end(); it++)
    {
        listChild.clear();
        listChild = GetObject(listParent, (*it).c_str());
        listParent.clear();

        if(0 == listChild.size())
        {
            return false;
        }
        listParent = listChild;
    }
    
    for(list<cJSON*>::iterator jt = listChild.begin(); jt != listChild.end(); jt++)
    {
        cJSON* pAttr = *jt;
        for(it = sfe.m_listValue.begin(); it != sfe.m_listValue.end(); it++)
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
                Remove(pAttr);
            }
            cJSON_Delete(pNumber);
            cJSON_Delete(pString);
        }
    }

    return nMatchedNum > 0;
}

list<cJSON*> CJsonFilter::GetObject(cJSON* pParent, const char *strAttrName)
{
    list<cJSON*> ret;

    if(NULL == pParent)
    {
        return ret;
    }

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

list<cJSON*> CJsonFilter::GetObject(list<cJSON*> &listParent, const char *strAttrName)
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

void CJsonFilter::Remove(cJSON* pNode)
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
