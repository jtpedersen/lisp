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
  const auto ast = p.read();
  ASSERT_NE(ast, nullptr);
  ASSERT_EQ(AST::Type::SYMBOL, ast->type());
  ASSERT_EQ(ast->children().size(), 0);
}

TEST(parser, String) {
  Lexer l("\"hest\"");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_NE(ast, nullptr);
  ASSERT_EQ(AST::Type::STRING, ast->type());
  ASSERT_EQ(ast->children().size(), 0);
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

TEST(parser, nestedSexpr) {
  Lexer l("( () () )");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_NE(ast, nullptr);
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(ast->children().size(), 2);
  EXPECT_EQ(AST::Type::SEXPR, (*ast)[0]->type()) << (*ast)[0]->type();
  EXPECT_EQ(AST::Type::SEXPR, (*ast)[1]->type()) << (*ast)[1]->type();
}

TEST(parser, multinestedSexpr) {
  Lexer l("(((())))");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_NE(ast, nullptr);
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(ast->children().size(), 1);
  EXPECT_EQ(AST::Type::SEXPR, (*ast)[0]->type()) << (*ast)[0]->type();
}

TEST(parser, unbalanced) {
  Lexer l("(((()))");
  Parser p(l);
  ASSERT_THROW(p.read(), SyntaxError);
}

TEST(parser, defineSexpr) {
  Lexer l("(define () () )");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::DEFINE, ast->type());
}

TEST(parser, defineMustHaveArgumentList) {
  Lexer l("(define )");
  Parser p(l);
  try {
    p.read();
    FAIL();
  } catch (SyntaxError &e) {
    const auto msg = "Define must have argumentlist and body";
    EXPECT_EQ(0, strncmp(msg, e.what(), strlen(msg)));
  }
}

TEST(parser, defineMustHaveBody) {
  Lexer l("(define () )");
  Parser p(l);
  try {
    p.read();
    FAIL();
  } catch (SyntaxError &e) {
    const auto msg = "Define must have a body";
    EXPECT_EQ(0, strncmp(msg, e.what(), strlen(msg)));
  }
}

TEST(parser, builtinAdd) {
  Lexer l("(+)");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(1, ast->children().size());
  const auto first = ast->children()[0];
  EXPECT_EQ(first->type(), AST::Type::BUILTIN);
  const auto node = std::static_pointer_cast<ASTBuiltin>(first);
  EXPECT_EQ(ASTBuiltin::Operator::ADD, node->op())
      << ASTBuiltin::operatorToCString(node->op());
}

TEST(parser, builtinSub) {
  Lexer l("(-)");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(1, ast->children().size());
  const auto first = ast->children()[0];
  EXPECT_EQ(first->type(), AST::Type::BUILTIN);
  const auto node = std::static_pointer_cast<ASTBuiltin>(first);
  EXPECT_EQ(ASTBuiltin::Operator::SUB, node->op())
      << ASTBuiltin::operatorToCString(node->op());
}

TEST(parser, builtinDiv) {
  Lexer l("(/)");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(1, ast->children().size());
  const auto first = ast->children()[0];
  EXPECT_EQ(first->type(), AST::Type::BUILTIN);
  const auto node = std::static_pointer_cast<ASTBuiltin>(first);
  EXPECT_EQ(ASTBuiltin::Operator::DIV, node->op())
      << ASTBuiltin::operatorToCString(node->op());
}

TEST(parser, builtinMul) {
  Lexer l("(*)");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(1, ast->children().size());
  const auto first = ast->children()[0];
  EXPECT_EQ(first->type(), AST::Type::BUILTIN);
  const auto node = std::static_pointer_cast<ASTBuiltin>(first);
  EXPECT_EQ(ASTBuiltin::Operator::MUL, node->op())
      << ASTBuiltin::operatorToCString(node->op());
}


TEST(parser, toStringInteger) {
  Lexer l("12");
  Parser p(l);
  const auto ast = p.read();
  EXPECT_STREQ("12", ast->toString());
}

TEST(parser, toStringSymbol) {
  Lexer l("hest");
  Parser p(l);
  const auto ast = p.read();
  EXPECT_STREQ("hest", ast->toString());
}

TEST(parser, toStringString) {
  Lexer l("\"hest\"");
  Parser p(l);
  const auto ast = p.read();
  EXPECT_STREQ("\"hest\"", ast->toString());
}




int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
