#include "gmock/gmock.h"
#include "gtest/gtest.h"


#include "Lexer.h"

TEST(lexer, none) {
  Lexer l("", 0);
  const auto token = l.nextToken();
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, token);
}

TEST(lexer, startParen) {
  Lexer l("(", 1);
  const auto token = l.nextToken();
  ASSERT_EQ(Lexer::TokenType::START_PAREN, token);
}

TEST(lexer, endParen) {
  Lexer l(")", 1);
  const auto token = l.nextToken();
  ASSERT_EQ(Lexer::TokenType::END_PAREN, token);
}

TEST(lexer, StartAndEndParen) {
  Lexer l("()", 1);
  ASSERT_EQ(Lexer::TokenType::START_PAREN, l.nextToken());
  ASSERT_EQ(Lexer::TokenType::END_PAREN, l.nextToken());
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, String) {
  const auto str = "\"hest\"";
  Lexer l(str, strlen(str));
  ASSERT_EQ(Lexer::TokenType::STRING, l.nextToken());
  ASSERT_STREQ(l.string(), "hest");
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, StringWithWhiteSpaceAndDigits) {
  const auto str = "\"hest 123\"";
  Lexer l(str, strlen(str));
  ASSERT_EQ(Lexer::TokenType::STRING, l.nextToken());
  ASSERT_STREQ(l.string(), "hest 123");
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, StringWithEscapeSequence) {
  const auto str = "\"\\\"'\"";
  Lexer l(str, strlen(str));
  ASSERT_EQ(Lexer::TokenType::STRING, l.nextToken());
  ASSERT_STREQ(l.string(), "\"'");
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

TEST(lexer, Symbol) {
  const auto str = "Symbol";
  Lexer l(str, strlen(str));
  ASSERT_EQ(Lexer::TokenType::SYMBOL, l.nextToken());
  ASSERT_STREQ(l.string(), "Symbol");
  ASSERT_EQ(Lexer::TokenType::TOKEN_EOF, l.nextToken());
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
