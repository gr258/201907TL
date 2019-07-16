#include "JsonFilter.h"

string JsonFilter(const char * jsonfile, const char * filterExpr)
{
    CJsonFilter jf(jsonfile,filterExpr);

    return jf.GetResult();
}

