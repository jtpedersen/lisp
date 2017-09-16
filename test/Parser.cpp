#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Parser.h"
#include "Util.h"
#include "SyntaxError.h"

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
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::DEFINE));
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
  Lexer l("(+ 1 2)");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(3, ast->children().size());
  const auto first = ast->children()[0];
  EXPECT_EQ(first->type(), AST::Type::BUILTIN);
  const auto node = std::static_pointer_cast<ASTBuiltin>(first);
  EXPECT_EQ(Builtin::ADD, node->op())
      << builtinToCString(node->op());
}

TEST(parser, builtinSub) {
  Lexer l("(- 1 2)");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(3, ast->children().size());
  const auto first = ast->children()[0];
  EXPECT_EQ(first->type(), AST::Type::BUILTIN);
  const auto node = std::static_pointer_cast<ASTBuiltin>(first);
  EXPECT_EQ(Builtin::SUB, node->op())
      << builtinToCString(node->op());
}

TEST(parser, builtinDiv) {
  Lexer l("(/ 10 10)");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(3, ast->children().size());
  const auto first = ast->children()[0];
  EXPECT_EQ(first->type(), AST::Type::BUILTIN);
  const auto node = std::static_pointer_cast<ASTBuiltin>(first);
  EXPECT_EQ(Builtin::DIV, node->op())
      << builtinToCString(node->op());
}

TEST(parser, builtinMul) {
  Lexer l("(* 0 0)");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(3, ast->children().size());
  const auto first = ast->children()[0];
  EXPECT_EQ(first->type(), AST::Type::BUILTIN);
  const auto node = std::static_pointer_cast<ASTBuiltin>(first);
  EXPECT_EQ(Builtin::MUL, node->op())
      << builtinToCString(node->op());
}


TEST(parser, builtinArgs) {
  Lexer l("(* 1 2 3)");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(4, ast->children().size());
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

TEST(parser, listBuiltin) {
  Lexer l("(list a b c)");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::LIST));
  ASSERT_EQ(4, ast->children().size());
}


TEST(parser, headBuiltin) {
  Lexer l("(head (list a))");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::HEAD));
  ASSERT_EQ(2, ast->children().size());
}

TEST(parser, headNeepsOperands) {
  Lexer l("(head)");
  Parser p(l);
  try {
    p.read();
    FAIL();
  } catch (SyntaxError &e) {
    const auto msg = "Builtin requires operands";
    EXPECT_EQ(0, strncmp(msg, e.what(), strlen(msg)));
  } catch (std::exception &e) {
    FAIL() << e.what() << " was not expected here";
  }
}


TEST(parser, tailBuiltin) {
  Lexer l("(tail (list a))");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::TAIL));
  ASSERT_EQ(2, ast->children().size());
}

TEST(parser, tailNeepsOperands) {
  Lexer l("(tail)");
  Parser p(l);
  try {
    p.read();
    FAIL();
  } catch (SyntaxError &e) {
    const auto msg = "Builtin requires operands";
    EXPECT_EQ(0, strncmp(msg, e.what(), strlen(msg)));
  } catch (std::exception &e) {
    FAIL() << e.what() << " was not expected here";
  }
}

TEST(parser, joinBuiltin) {
  Lexer l("(join (list a) (list b))");
  Parser p(l);
  const auto ast = p.read();
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::JOIN));
  ASSERT_EQ(3, ast->children().size());
}



int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
