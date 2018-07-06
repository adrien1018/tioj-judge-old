#include <set>
#include <vector>
#include <gtest/gtest.h>

#include "utils.h"

namespace {

TEST(ValidNameTest, Length) {
  EXPECT_TRUE(IsValidName("a"));
  EXPECT_FALSE(IsValidName(""));
  EXPECT_TRUE(IsValidName("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijkl"));
  EXPECT_FALSE(IsValidName("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklm"));
}

TEST(ValidNameTest, Charset) {
  EXPECT_FALSE(IsValidName("1a"));
  EXPECT_FALSE(IsValidName("a-a"));
  EXPECT_FALSE(IsValidName("a&a"));
  EXPECT_FALSE(IsValidName("a)a"));
  EXPECT_FALSE(IsValidName("喵"));
  EXPECT_TRUE(IsValidName("____"));
  EXPECT_TRUE(IsValidName("abcdefghijklmnopqrstuvwxyz_0123456789"));
  EXPECT_TRUE(IsValidName("ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"));
}

TEST(DateTimeStrTest, Main) {
  EXPECT_EQ("1970-01-01 00:00:00.000", DateTimeStr(62167219200000));
  EXPECT_EQ("1969-12-31 23:59:59.999", DateTimeStr(62167219199999));
  EXPECT_EQ("2018-06-12 06:15:59.013", DateTimeStr(63696003359013));
  EXPECT_EQ("2018-10-26 06:15:05.003", DateTimeStr(63707753705003));
  EXPECT_EQ("1718-06-12 06:15:59.213", DateTimeStr(54228896159213));
  EXPECT_EQ("2388-03-01 08:03:01.831", DateTimeStr(75363206581831));
}

TEST(DateTimeValTest, Main) {
  EXPECT_EQ(DateTimeVal("1970-01-01 00:00:00.000"), 62167219200000);
  EXPECT_EQ(DateTimeVal("1969-12-31 23:59:59.999"), 62167219199999);
  EXPECT_EQ(DateTimeVal("2018-06-12 06:15:59.013"), 63696003359013);
  EXPECT_EQ(DateTimeVal("2018-10-26 06:15:05.003"), 63707753705003);
  EXPECT_EQ(DateTimeVal("1718-06-12 06:15:59.213"), 54228896159213);
  EXPECT_EQ(DateTimeVal("2388-03-01 08:03:01.831"), 75363206581831);
  EXPECT_THROW(DateTimeVal("2388-03-01 08:03:01.8311"), std::invalid_argument);
  EXPECT_THROW(DateTimeVal("2388-3-01 08:03:01.811"), std::invalid_argument);
  EXPECT_THROW(DateTimeVal("388-03-01 08:03:01.811"), std::invalid_argument);
}

typedef std::vector<std::string> StrArray;

TEST(SplitStringTest, Basic) {
  EXPECT_EQ(StrArray({"1", "2", "3", "4", "5"}), SplitString("1,2,3,4,5"));
  EXPECT_EQ(StrArray({"123"}), SplitString("123"));
  EXPECT_EQ(StrArray(), SplitString(""));
}

TEST(SplitStringTest, SpecialChar) {
  EXPECT_EQ(StrArray({"1", "2\"", "2a"}), SplitString("1,\"2\"\"\",2a"));
  EXPECT_EQ(StrArray({"1", "2\n", "2a,"}), SplitString("1,\"2\n\",\"2a,\""));
  EXPECT_EQ(StrArray({"喔", "哈"}), SplitString("喔,哈"));
}

TEST(MergeStringTest, String) {
  StrArray vec;
  vec = StrArray({"1", "2", "3", "4", "5"});
  EXPECT_EQ("1,2,3,4,5", MergeString(vec.begin(), vec.end()));
  EXPECT_EQ("1,2,3,4,5", MergeString(vec));
  vec.clear();
  EXPECT_EQ("", MergeString(vec));
  EXPECT_EQ("\"\n\",\"123\"\"123\",\",,,\"",
      MergeString(StrArray({"\n", "123\"123", ",,,"})));
}

TEST(MergeStringTest, Func) {
  std::set<int> a({3,7,4,5,2,6,1,7});
  auto IntToStr = [](int a){return std::to_string(a);};
  EXPECT_EQ("1,2,3,4,5,6,7", MergeString(a.begin(), a.end(), IntToStr));
  EXPECT_EQ("1,2,3,4,5,6,7", MergeString(a, IntToStr));
}

} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
