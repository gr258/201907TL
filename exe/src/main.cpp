#include <stdio.h>
#include "JsonFilterApi.h"

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Usage: %s [jsonfile] [filterExpr]\n",argv[0]);
    }
    else
    {
        string strResult = JsonFilter(argv[1], argv[2]);
        printf("%s\n", strResult.c_str());
    }
    return 0;
}

