#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Parser.h"
#include "SyntaxError.h"
#include "Util.h"

class ParserTest : public ::testing::Test {

protected:
  std::shared_ptr<AST> readFirst(const char *code) {
    Lexer l(code);
    Parser p(l);
    const auto program = p.read();
    EXPECT_GT(program.size(), 0);
    EXPECT_NE(nullptr, program[0]);
    return program[0];
  }
};

TEST_F(ParserTest, minimal) {
  const auto ast = readFirst("()");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  EXPECT_EQ(ast->children().size(), 0);
}

TEST_F(ParserTest, symbol) {
  const auto ast = readFirst("hest");
  ASSERT_EQ(AST::Type::SYMBOL, ast->type());
  ASSERT_EQ(ast->children().size(), 0);
}

TEST_F(ParserTest, String) {
  const auto ast = readFirst("\"hest\"");
  ASSERT_EQ(AST::Type::STRING, ast->type());
  ASSERT_EQ(ast->children().size(), 0);
}

TEST_F(ParserTest, singleSymbolChild) {
  const auto ast = readFirst("(hest)");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(ast->children().size(), 1);
  EXPECT_EQ(AST::Type::SYMBOL, (*ast)[0]->type()) << (*ast)[0]->type();
}

TEST_F(ParserTest, singleIntegerChild) {
  const auto ast = readFirst("( 2)");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(ast->children().size(), 1);
  EXPECT_EQ(AST::Type::INTEGER, (*ast)[0]->type()) << (*ast)[0]->type();
}

TEST_F(ParserTest, symbolAndInteger) {
  const auto ast = readFirst("( hest 12 )");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(ast->children().size(), 2);
  EXPECT_EQ(AST::Type::SYMBOL, (*ast)[0]->type()) << (*ast)[0]->type();
  EXPECT_EQ(AST::Type::INTEGER, (*ast)[1]->type()) << (*ast)[1]->type();
}

TEST_F(ParserTest, booleanTrue) {
  const auto ast = readFirst("true");
  EXPECT_EQ(AST::Type::BOOLEAN, ast->type());
}

TEST_F(ParserTest, booleanFalse) {
  const auto ast = readFirst("false");
  EXPECT_EQ(AST::Type::BOOLEAN, ast->type());
}

TEST_F(ParserTest, nestedSexpr) {
  const auto ast = readFirst("( () () )");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(ast->children().size(), 2);
  EXPECT_EQ(AST::Type::SEXPR, (*ast)[0]->type()) << (*ast)[0]->type();
  EXPECT_EQ(AST::Type::SEXPR, (*ast)[1]->type()) << (*ast)[1]->type();
}

TEST_F(ParserTest, multinestedSexpr) {
  const auto ast = readFirst("(((())))");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(ast->children().size(), 1);
  EXPECT_EQ(AST::Type::SEXPR, (*ast)[0]->type()) << (*ast)[0]->type();
}

TEST_F(ParserTest, unbalanced) {
  Lexer l("(((()))");
  Parser p(l);
  ASSERT_THROW(p.read(), SyntaxError);
}

TEST_F(ParserTest, builtinAdd) {
  const auto ast = readFirst("(+ 1 2)");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(3, ast->children().size());
  const auto first = ast->children()[0];
  EXPECT_EQ(first->type(), AST::Type::BUILTIN);
  const auto node = std::static_pointer_cast<ASTBuiltin>(first);
  EXPECT_EQ(Builtin::ADD, node->op()) << builtinToCString(node->op());
}

TEST_F(ParserTest, builtinSub) {
  const auto ast = readFirst("(- 1 2)");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(3, ast->children().size());
  const auto first = ast->children()[0];
  EXPECT_EQ(first->type(), AST::Type::BUILTIN);
  const auto node = std::static_pointer_cast<ASTBuiltin>(first);
  EXPECT_EQ(Builtin::SUB, node->op()) << builtinToCString(node->op());
}

TEST_F(ParserTest, builtinDiv) {
  const auto ast = readFirst("(/ 10 10)");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(3, ast->children().size());
  const auto first = ast->children()[0];
  EXPECT_EQ(first->type(), AST::Type::BUILTIN);
  const auto node = std::static_pointer_cast<ASTBuiltin>(first);
  EXPECT_EQ(Builtin::DIV, node->op()) << builtinToCString(node->op());
}

TEST_F(ParserTest, builtinMul) {
  const auto ast = readFirst("(* 0 0)");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(3, ast->children().size());
  const auto first = ast->children()[0];
  EXPECT_EQ(first->type(), AST::Type::BUILTIN);
  const auto node = std::static_pointer_cast<ASTBuiltin>(first);
  EXPECT_EQ(Builtin::MUL, node->op()) << builtinToCString(node->op());
}

TEST_F(ParserTest, builtinArgs) {
  const auto ast = readFirst("(* 1 2 3)");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_EQ(4, ast->children().size());
}

TEST_F(ParserTest, toStringInteger) {
  const auto ast = readFirst("12");
  EXPECT_STREQ("12", ast->toString());
}

TEST_F(ParserTest, toStringSymbol) {
  const auto ast = readFirst("hest");
  EXPECT_STREQ("hest", ast->toString());
}

TEST_F(ParserTest, toStringString) {
  const auto ast = readFirst("\"hest\"");
  EXPECT_STREQ("\"hest\"", ast->toString());
}

TEST_F(ParserTest, listBuiltin) {
  const auto ast = readFirst("(list a b c)");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::LIST));
  ASSERT_EQ(4, ast->children().size());
}

TEST_F(ParserTest, headBuiltin) {
  const auto ast = readFirst("(head (list a))");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::HEAD));
  ASSERT_EQ(2, ast->children().size());
}

TEST_F(ParserTest, headNeepsOperands) {
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

TEST_F(ParserTest, tailBuiltin) {
  const auto ast = readFirst("(tail (list a))");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::TAIL));
  ASSERT_EQ(2, ast->children().size());
}

TEST_F(ParserTest, tailNeepsOperands) {
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

TEST_F(ParserTest, joinBuiltin) {
  const auto ast = readFirst("(join (list a) (list b))");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::JOIN));
  ASSERT_EQ(3, ast->children().size());
}

TEST_F(ParserTest, joinNeedsOperands) {
  Lexer l("(join)");
  Parser p(l);
  try {
    p.read();
    FAIL();
  } catch (SyntaxError &e) {
    const auto msg = "Builtin requires two operands";
    EXPECT_EQ(0, strncmp(msg, e.what(), strlen(msg))) << e.what();
  } catch (std::exception &e) {
    FAIL() << e.what() << " was not expected here";
  }
}

TEST_F(ParserTest, joinNeedsMoreThanOneOperand) {
  Lexer l("(join (list a))");
  Parser p(l);
  try {
    p.read();
    FAIL();
  } catch (SyntaxError &e) {
    const auto msg = "Builtin requires two operands";
    EXPECT_EQ(0, strncmp(msg, e.what(), strlen(msg)));
  } catch (std::exception &e) {
    FAIL() << e.what() << " was not expected here";
  }
}

TEST_F(ParserTest, evalBuiltin) {
  const auto ast = readFirst("(eval (list + 1 2))");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::EVAL));
  ASSERT_EQ(2, ast->children().size());
}

TEST_F(ParserTest, pprintBuiltin) {
  const auto ast = readFirst("(pprint 2)");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::PPRINT));
  ASSERT_EQ(2, ast->children().size());
}

TEST_F(ParserTest, defineBuiltin) {
  const auto ast = readFirst("(define x 2)");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::DEFINE));
  ASSERT_EQ(3, ast->children().size());
}

TEST_F(ParserTest, defineSexpr) {
  const auto ast = readFirst("(define y () )");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::DEFINE));
}

TEST_F(ParserTest, defineMustHaveName) {
  Lexer l("(define )");
  Parser p(l);
  try {
    p.read();
    FAIL();
  } catch (SyntaxError &e) {
    const auto msg = "Definition must have argumentlist and body";
    EXPECT_EQ(0, strncmp(msg, e.what(), strlen(msg)));
  }
}

TEST_F(ParserTest, defineMustHaveBody) {
  Lexer l("(define (name) )");
  Parser p(l);
  try {
    p.read();
    FAIL();
  } catch (SyntaxError &e) {
    const auto msg = "Definition must have a body";
    EXPECT_EQ(0, strncmp(msg, e.what(), strlen(msg)));
  }
}

TEST_F(ParserTest, defineVarMustHaveBody) {
  Lexer l("(define x )");
  Parser p(l);
  try {
    p.read();
    FAIL();
  } catch (SyntaxError &e) {
    const auto msg = "Definition must have a body";
    EXPECT_EQ(0, strncmp(msg, e.what(), strlen(msg)));
  }
}

TEST_F(ParserTest, defineFunSexpr) {
  const auto ast = readFirst("(define (name) (+ 1 2) )");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::DEFINE))
      << ast->head()->toString();
}

TEST_F(ParserTest, defineFunSexprWithArgs) {
  const auto ast = readFirst("(define (name x y) (* x y) )");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::DEFINE))
      << ast->head()->toString();
}

TEST_F(ParserTest, defineIf) {
  const auto ast = readFirst("(if (predicate) (yes) (no) )");
  ASSERT_EQ(AST::Type::SEXPR, ast->type());
  ASSERT_TRUE(ast->head()->isBuiltin(Builtin::IF)) << ast->head()->toString();
}

TEST_F(ParserTest, ifMustHaveCondBranches) {
  Lexer l("(if )");
  Parser p(l);
  try {
    p.read();
    FAIL();
  } catch (SyntaxError &e) {
    const auto msg = "If-expression must har predicate and such";
    EXPECT_EQ(0, strncmp(msg, e.what(), strlen(msg)));
  }
}

TEST_F(ParserTest, operatorEq) {
  const auto ast = readFirst("=");
  ASSERT_TRUE(ast->isBuiltin(Builtin::EQ));
}

TEST_F(ParserTest, operatorGT) {
  const auto ast = readFirst(">");
  ASSERT_TRUE(ast->isBuiltin(Builtin::GT));
}

TEST_F(ParserTest, operatorGE) {
  const auto ast = readFirst(">=");
  ASSERT_TRUE(ast->isBuiltin(Builtin::GE));
}

TEST_F(ParserTest, operatorLT) {
  const auto ast = readFirst("<");
  ASSERT_TRUE(ast->isBuiltin(Builtin::LT));
}

TEST_F(ParserTest, operatorLE) {
  const auto ast = readFirst("<=");
  ASSERT_TRUE(ast->isBuiltin(Builtin::LE));
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
