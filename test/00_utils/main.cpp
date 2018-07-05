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

} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
