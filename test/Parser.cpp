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
  EXPECT_EQ(ast->children().size(), 0);
}

TEST(parser, symbol) {
  Lexer l("hest");
  Parser p(l);
  ASSERT_THROW(p.read(), SyntaxError);
}

TEST(parser, singleSymbolChild) {
  Lexer l("(hest)");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_NE(ast, nullptr);
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(ast->children().size(), 1);
  EXPECT_EQ(AST::Type::SYMBOL, (*ast)[0]->type()) << (*ast)[0]->type();
}

TEST(parser, singleIntegerChild) {
  Lexer l("( 2)");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_NE(ast, nullptr);
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(ast->children().size(), 1);
  EXPECT_EQ(AST::Type::INTEGER, (*ast)[0]->type()) << (*ast)[0]->type();
}

TEST(parser, symbolAndInteger) {
  Lexer l("( hest 12 )");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_NE(ast, nullptr);
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(ast->children().size(), 2);
  EXPECT_EQ(AST::Type::SYMBOL, (*ast)[0]->type()) << (*ast)[0]->type();
  EXPECT_EQ(AST::Type::INTEGER, (*ast)[1]->type()) << (*ast)[1]->type();
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
