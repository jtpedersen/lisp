#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Parser.h"
#include "Util.h"

TEST(parser, minimal) {
  Lexer l("()");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_NE(ast, nullptr);
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
}


TEST(parser, symbol) {
  Lexer l("hest");
  Parser p(l);
  ASSERT_THROW(p.read(), SyntaxError);
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
