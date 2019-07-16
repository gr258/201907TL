#include "gtest/gtest.h"
#include "JsonFilterApi.h"
#include "cJSON.h"

string Normalized(const char *strText)
{
    string strJson = "[]";

    cJSON* pJsonData = cJSON_Parse(strText);
    if(NULL == pJsonData)
        return strJson;

    char* pJsonString = cJSON_PrintUnformatted(pJsonData);
    if (pJsonString != NULL)
    {
        strJson = pJsonString;
        cJSON_free(pJsonString);
    }
    cJSON_Delete(pJsonData);
    return strJson;
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_WEIGHT_EQ_100_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":123, \"weight\":100, \"parts\":[{\"id\":1, \"color\":\"red\"}, {\"id\":2, \"color\":\"green\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","weight.eq=100"));
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","weight=100"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_COLOR_EQ_GREEN_AND_ID_EQ_3_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":3, \"color\":\"green\"}, {\"id\":2, \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color=green&parts.id=3"));
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.eq=green&parts.id.eq=3"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_COLOR_EQ_GREEN_AND_ID_EQ_2_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":123, \"weight\":100, \"parts\":[{\"id\":1, \"color\":\"red\"}, {\"id\":2, \"color\":\"green\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color=green&parts.id=2"));
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.eq=green&parts.id.eq=2"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_COLOR_EQ_GREEN_AND_ID_EQ_1_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color=green&parts.id=1"));
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.eq=green&parts.id.eq=1"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_WEIGHT_EQ_100_500_THEN_RETURN_TWO)
{
    string expect = Normalized("[\n{\"id\":123, \"weight\":100, \"parts\":[{\"id\":1, \"color\":\"red\"}, {\"id\":2, \"color\":\"green\"}]},\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":3, \"color\":\"green\"}, {\"id\":2, \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","weight.eq=100,500"));
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","weight=100,500"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_WEIGHT_NEQ_100_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":3, \"color\":\"green\"}, {\"id\":2, \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","weight.neq=100"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_COLOR_NEQ_GREEN_AND_ID_NEQ_2_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":123, \"weight\":100, \"parts\":[{\"id\":1, \"color\":\"red\"}, {\"id\":2, \"color\":\"green\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.neq=green&parts.id.neq=2"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_COLOR_NEQ_GREEN_AND_ID_EQ_3_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.neq=green&parts.id.eq=3"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_COLOR_NEQ_RED_GREEN_BLUE_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n{\"id\":123, \"weight\":100, \"parts\":[{\"id\":1, \"color\":\"red\"}, {\"id\":2, \"color\":\"green\"}]},\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":3, \"color\":\"green\"}, {\"id\":2, \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.neq=red,green,blue"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_WEIGHT_GT_100_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":3, \"color\":\"green\"}, {\"id\":2, \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","weight.gt=100"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_ID_GT_2_1_THEN_RETURN_TWO)
{
    string expect = Normalized("[\n{\"id\":123, \"weight\":100, \"parts\":[{\"id\":1, \"color\":\"red\"}, {\"id\":2, \"color\":\"green\"}]},\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":3, \"color\":\"green\"}, {\"id\":2, \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.id.gt=2,1"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_ID_GT_2_1_AND_COLOR_GT_GREEN_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":3, \"color\":\"green\"}, {\"id\":2, \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.id.gt=2,1&parts.color.gt=green"));
}


