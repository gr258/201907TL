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
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.neq=red,green,blue"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_WEIGHT_GT_100_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":3, \"color\":\"green\"}, {\"id\":2, \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","weight.gt=100"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_COLOR_GT_GREEN_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":123, \"weight\":100, \"parts\":[{\"id\":1, \"color\":\"red\"}, {\"id\":2, \"color\":\"green\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.gt=green"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_ID_GT_2_1_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.id.gt=2,1"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_WEIGHT_GTE_100_THEN_RETURN_TWO)
{
    string expect = Normalized("[\n{\"id\":123, \"weight\":100, \"parts\":[{\"id\":1, \"color\":\"red\"}, {\"id\":2, \"color\":\"green\"}]},\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":3, \"color\":\"green\"}, {\"id\":2, \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","weight.gte=100"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_COLOR_GTE_GREEN_THEN_RETURN_TWO)
{
    string expect = Normalized("[\n{\"id\":123, \"weight\":100, \"parts\":[{\"id\":1, \"color\":\"red\"}, {\"id\":2, \"color\":\"green\"}]},\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":3, \"color\":\"green\"}, {\"id\":2, \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.gte=green"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_COLOR_GTE_GREEN_BLUE_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.gte=green,blue"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_WEIGHT_LT_100_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","weight.lt=100"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_COLOR_LT_GREEN_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":3, \"color\":\"green\"}, {\"id\":2, \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.lt=green"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_ID_LT_1_2_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.id.lt=1,2"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_WEIGHT_LTE_100_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":123, \"weight\":100, \"parts\":[{\"id\":1, \"color\":\"red\"}, {\"id\":2, \"color\":\"green\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","weight.lte=100"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_COLOR_LTE_BLUE_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":3, \"color\":\"green\"}, {\"id\":2, \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.lte=blue"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_ID_LTE_1_2_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.id.lte=1,2"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_EMPTY_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json",""));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_VALID_AND_INVALID_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","weight.eq=100&invalid.filter"));
}

TEST(JSONFILTER,GIVEN_ID_TYPE_ARRAY_JSON_FILE_WHEN_FILTER_IS_ID_CONT_8_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":[3,6], \"color\":\"green\"}, {\"id\":[2,8], \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/id_type_array.json","parts.id.cont=8"));
}

TEST(JSONFILTER,GIVEN_ID_TYPE_ARRAY_JSON_FILE_WHEN_FILTER_IS_ID_CONT_4_8_THEN_RETURN_TWO)
{
    string expect = Normalized("[\n{\"id\":123, \"weight\":100, \"parts\":[{\"id\":[1, 5], \"color\":\"red\"}, {\"id\":[2, 4], \"color\":\"green\"}]}, {\"id\":456, \"weight\":500, \"parts\":[{\"id\":[3, 6], \"color\":\"green\"}, {\"id\":[2, 8], \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/id_type_array.json","parts.id.cont=4,8"));
}

TEST(JSONFILTER,GIVEN_ID_TYPE_ARRAY_JSON_FILE_WHEN_FILTER_IS_ID_CONT_4_8_AND_COLOR_EQ_GREEN_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":123, \"weight\":100, \"parts\":[{\"id\":[1, 5], \"color\":\"red\"}, {\"id\":[2, 4], \"color\":\"green\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/id_type_array.json","parts.id.cont=4,8&parts.color.eq=green"));
}

TEST(JSONFILTER,GIVEN_ID_TYPE_ARRAY_JSON_FILE_WHEN_FILTER_IS_ID_CONT_1_8_AND_COLOR_EQ_GREEN_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/id_type_array.json","parts.id.cont=1,8&parts.color.eq=green"));
}

TEST(JSONFILTER,GIVEN_ID_TYPE_ARRAY_JSON_FILE_WHEN_FILTER_IS_ID_NCONT_1_2_3_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/id_type_array.json","parts.id.ncont=1,2,3"));
}

TEST(JSONFILTER,GIVEN_ID_TYPE_ARRAY_JSON_FILE_WHEN_FILTER_IS_ID_NCONT_4_5_6_THEN_RETURN_ONE)
{
    string expect = Normalized("[\n{\"id\":456, \"weight\":500, \"parts\":[{\"id\":[3,6], \"color\":\"green\"}, {\"id\":[2,8], \"color\":\"blue\"}]}\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/id_type_array.json","parts.id.ncont=4,5,6"));
}

TEST(JSONFILTER,GIVEN_ID_TYPE_ARRAY_JSON_FILE_WHEN_FILTER_IS_COLOR_CONT_GREEN_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/id_type_array.json","parts.color.cont=green"));
}

TEST(JSONFILTER,GIVEN_TWO_ARRAY_JSON_FILE_WHEN_FILTER_IS_COLOR_EQ_GREEN_AND_TYPE_EQ_5_THEN_RETURN_ONE)
{
    string expect = Normalized("[{\"id\":123,\"weight\":100,\"parts\":[{\"id\":1,\"color\":\"red\"},{\"id\":2,\"color\":\"green\"}],\"elements\":[{\"name\":\"abc\",\"type\":\"5\"},{\"name\":\"def\",\"type\":\"6\"}]}]");
    EXPECT_EQ(expect,JsonFilter("../test/file/two_array.json","parts.color.eq=green&elements.type.eq=5"));
}

TEST(JSONFILTER,GIVEN_TWO_ARRAY_JSON_FILE_WHEN_FILTER_IS_COLOR_LT_GREEN_AND_NAME_NEQ_ABC_THEN_RETURN_ONE)
{
    string expect = Normalized("[{\"id\":456,\"weight\":500,\"parts\":[{\"id\":3,\"color\":\"green\"},{\"id\":2,\"color\":\"blue\"}],\"elements\":[{\"name\":\"hello\",\"type\":\"6\"},{\"name\":\"world\",\"type\":\"7\"}]}]");
    EXPECT_EQ(expect,JsonFilter("../test/file/two_array.json","parts.color.lt=green&elements.name.neq=abc"));
}

TEST(JSONFILTER,GIVEN_TWO_ARRAY_JSON_FILE_WHEN_FILTER_IS_ID_LT_1_AND_NAME_NEQ_ANY_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/two_array.json","parts.id.lt=1&elements.name.neq=any"));
}

TEST(JSONFILTER,GIVEN_OBJECT_IN_ARRAY_JSON_FILE_WHEN_FILTER_IS_NAME_EQ_CAR_AND_COLOR_EQ_RED_THEN_RETURN_ONE)
{
    string expect = Normalized("[{\"id\":123,\"weight\":100,\"parts\":[{\"id\":1,\"color\":\"red\",\"item\":{\"name\":\"car\"}},{\"id\":2,\"color\":\"green\",\"item\":{\"name\":\"ship\"}}]}]");
    EXPECT_EQ(expect,JsonFilter("../test/file/object_in_array.json","parts.item.name.eq=car&parts.color.eq=red"));
}

TEST(JSONFILTER,GIVEN_OBJECT_IN_ARRAY_JSON_FILE_WHEN_FILTER_IS_NAME_EQ_CAR_AND_COLOR_EQ_GREEN_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/object_in_array.json","parts.item.name.eq=car&parts.color.eq=green"));
}

TEST(JSONFILTER,GIVEN_ARRAY_IN_ARRAY_JSON_FILE_WHEN_FILTER_IS_NAME_EQ_CAR_AND_COLOR_EQ_RED_THEN_RETURN_NONE)
{
    string expect = Normalized("[{\"id\":123,\"weight\":100,\"parts\":[{\"id\":1,\"color\":\"red\",\"item\":[{\"name\":\"car\"},{\"name\":\"vehicle\"}]},{\"id\":2,\"color\":\"green\",\"item\":[{\"name\":\"ship\"},{\"name\":\"boat\"}]}]}]");
    EXPECT_EQ(expect,JsonFilter("../test/file/array_in_array.json","parts.item.name.eq=car&parts.color.eq=red"));
}

TEST(JSONFILTER,GIVEN_ARRAY_IN_ARRAY_JSON_FILE_WHEN_FILTER_IS_NAME_EQ_CAR_AND_COLOR_EQ_GREEN_THEN_RETURN_NONE)
{
    string expect = Normalized("[\n]");
    EXPECT_EQ(expect,JsonFilter("../test/file/array_in_array.json","parts.item.name.eq=car&parts.color.eq=green"));
}


