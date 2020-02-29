#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Util.h"


TEST(repl, none) {
  std::string data("abe");
  ASSERT_STREQ(data.c_str(), util::zap_to_keyword(data, 1).c_str());
}

TEST(repl, wrongWay) {
  std::string data("d)abe)");
  ASSERT_STREQ(data.c_str(), util::zap_to_keyword(data, 1).c_str());
}


TEST(repl, first) {
  std::string data("(abe");
  ASSERT_STREQ("abe", util::zap_to_keyword(data, 1).c_str());
}

TEST(repl, first_index_zero) {
  std::string data("(abe");
  ASSERT_STREQ("abe", util::zap_to_keyword(data, 0).c_str());
}


TEST(repl, last) {
  std::string data("ana(");
  ASSERT_STREQ("", util::zap_to_keyword(data, 1).c_str());
}


TEST(repl, second) {
  std::string data("a(na(b");
  ASSERT_STREQ("b", util::zap_to_keyword(data, 3).c_str());
}


TEST(repl, zeros) {
  std::string data("a(na");
  ASSERT_STREQ("na", util::zap_to_keyword(data, 0).c_str());
}

TEST(repl, whitespace) {
  std::string data("a na");
  ASSERT_STREQ("na", util::zap_to_keyword(data, 0).c_str());
}



int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
