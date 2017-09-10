#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Lexer.h"

TEST(lexer, none) {
  Lexer l("");
  const auto token = l.nextToken();
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, token);
}

TEST(lexer, startParen) {
  Lexer l("(");
  const auto token = l.nextToken();
  ASSERT_EQ(Lexer::TokenType::START_PAREN, token);
}

TEST(lexer, endParen) {
  Lexer l(")");
  const auto token = l.nextToken();
  ASSERT_EQ(Lexer::TokenType::END_PAREN, token);
}

TEST(lexer, StartAndEndParen) {
  Lexer l("()");
  ASSERT_EQ(Lexer::TokenType::START_PAREN, l.nextToken());
  ASSERT_EQ(Lexer::TokenType::END_PAREN, l.nextToken());
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, String) {
  const auto str = "\"hest\"";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::STRING, l.nextToken());
  ASSERT_STREQ(l.string(), "hest");
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, StringWithWhiteSpaceAndDigits) {
  const auto str = "\"hest 123\"";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::STRING, l.nextToken());
  ASSERT_STREQ(l.string(), "hest 123");
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, StringWithEscapeSequence) {
  const auto str = "\"\\\"'\"";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::STRING, l.nextToken());
  ASSERT_STREQ(l.string(), "\"'");
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, Symbol) {
  const auto str = "Symbol";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::SYMBOL, l.nextToken());
  ASSERT_STREQ(l.string(), "Symbol");
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, SymbolList) {
  const auto str = "alpha beta";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::SYMBOL, l.nextToken());
  ASSERT_STREQ(l.string(), "alpha");
  ASSERT_EQ(Lexer::TokenType::SYMBOL, l.nextToken());
  ASSERT_STREQ(l.string(), "beta");
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, integer) {
  const auto str = "123";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::INTEGER, l.nextToken());
  ASSERT_STREQ(l.string(), "123");
  ASSERT_EQ(l.integer(), 123);
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, negativeInteger) {
  const auto str = "-123";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::INTEGER, l.nextToken());
  ASSERT_STREQ(l.string(), "-123");
  ASSERT_EQ(l.integer(), -123);
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, location) {
  const auto str = "1";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::INTEGER, l.nextToken());
  EXPECT_EQ(l.line(), 1);
  EXPECT_EQ(l.col(), 1);
}

TEST(lexer, colAdvance) {
  const auto str = "1 1";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::INTEGER, l.nextToken());
  EXPECT_EQ(l.line(), 1);
  EXPECT_EQ(l.col(), 1);
  ASSERT_EQ(Lexer::TokenType::INTEGER, l.nextToken());
  EXPECT_EQ(l.col(), 3);
}

TEST(lexer, parensRules) {
  const auto str = "(hest)";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::START_PAREN, l.nextToken());
  ASSERT_EQ(Lexer::TokenType::SYMBOL, l.nextToken());
  EXPECT_STREQ("hest", l.string()) << l.string();

  ASSERT_EQ(Lexer::TokenType::END_PAREN, l.nextToken());
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, ignoreWhiteSpace) {
  const auto str = "(      hest          )";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::START_PAREN, l.nextToken());
  ASSERT_EQ(Lexer::TokenType::SYMBOL, l.nextToken());
  EXPECT_STREQ("hest", l.string()) << l.string();

  ASSERT_EQ(Lexer::TokenType::END_PAREN, l.nextToken());
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, symbol) {
  const auto str = "+";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::SYMBOL, l.nextToken());
  EXPECT_STREQ("+", l.string()) << l.string();
}

TEST(lexer, singleChar) {
  const auto str = "a";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::SYMBOL, l.nextToken());
  EXPECT_STREQ("a", l.string()) << l.string();
}

TEST(lexer, twoChar) {
  const auto str = "ab";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::SYMBOL, l.nextToken());
  EXPECT_STREQ("ab", l.string()) << l.string();
}

TEST(lexer, minusIsSymbol) {
  const auto str = "-";
  Lexer l(str);
  ASSERT_EQ(Lexer::TokenType::SYMBOL, l.nextToken());
  EXPECT_STREQ("-", l.string()) << l.string();
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
