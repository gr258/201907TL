#include "gtest/gtest.h"
#include "jsonfilterapi.h"

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_WEIGHT_EQ_THEN_RETURN_ONE)
{
    string expect = "[\n{\"id\":123,\"weight\":100,\"parts\":[{\"id\":1,\"color\":\"red\"},{\"id\":2,\"color\":\"green\"}]}\n]";
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","weight.eq=100"));
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","weight=100"));
}

TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_COLOR_AND_ID_EQ_THEN_RETURN_ONE)
{
    string expect = "[\n{\"id\":456,\"weight\":500,\"parts\":[{\"id\":3,\"color\":\"green\"},{\"id\":2,\"color\":\"blue\"}]}\n]";
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color=green&parts.id=3"));
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.eq=green&parts.id.eq=3"));
}

/*TEST(JSONFILTER,GIVEN_BASIC_JSON_FILE_WHEN_FILTER_IS_COLOR_AND_ID2_EQ_THEN_RETURN_ONE)
{
    string expect = "[\n{\"id\":123,\"weight\":100,\"parts\":[{\"id\":1,\"color\":\"red\"},{\"id\":2,\"color\":\"green\"}]}\n]";
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color=green&parts.id=2"));
    EXPECT_EQ(expect,JsonFilter("../test/file/basic.json","parts.color.eq=green&parts.id.eq=2"));
}*/
