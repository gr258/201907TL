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

bool CJsonFilter::Parse(string &strJson, string &strFilter)
{
    m_pJsonData = cJSON_Parse(strJson.c_str());
    if (m_pJsonData == NULL)
    {
        m_strErrMsg = string("prase json string error at ") + cJSON_GetErrorPtr();
        return false;
    }
    
    size_t start = 0, end = 0;
    end = strFilter.find('&', start);
    while(end != string::npos)
    {
        m_listSimpleFilterExpr.push_back(strFilter.substr(start,end-start));
        start = end + 1;
        end = strFilter.find('&', start);
    }
    if(start >= strFilter.size())
    {
        m_strErrMsg = string("prase filter string error");
        return false;
    }
    m_listSimpleFilterExpr.push_back(strFilter.substr(start,strFilter.size()-start));

    return true;
}

void CJsonFilter::Clear()
{
    if (m_pJsonData != NULL)
    {
        cJSON_Delete(m_pJsonData);
        m_pJsonData = NULL;
    }
    m_listSimpleFilterExpr.clear();
}

string CJsonFilter::GetResult()
{
    m_strResult = "[\n";
    if (m_pJsonData != NULL && m_pJsonData->type == cJSON_Array)
    {
        int nSize = cJSON_GetArraySize(m_pJsonData);
        for(int i = 0; i < nSize; i++)
        {
            cJSON* pJsonStruct = cJSON_GetArrayItem(m_pJsonData, i);
            char* pJsonString = cJSON_PrintUnformatted(pJsonStruct);
            if (pJsonString != NULL)
            {
                m_strResult += pJsonString + string("\n");
                free(pJsonString);
            }
        }
    }
    m_strResult += "]";
    
    return m_strResult;
}

void CJsonFilter::PrintResult()
{
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

