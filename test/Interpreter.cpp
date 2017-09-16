#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "SyntaxError.h"
#include "Util.h"

class InterpreterTest : public ::testing::Test {

protected:
  void load(const char *code) {
    Lexer l(code);
    Parser p(l);
    program = p.read();
  }
  std::shared_ptr<AST> program;
};

TEST_F(InterpreterTest, minimal) {
  load("12");
  const auto res = eval(program);
  EXPECT_EQ(program->type(), res->type());
  EXPECT_STREQ(program->toString(), res->toString());
}

TEST_F(InterpreterTest, minimalEmpty) {
  load("()");
  const auto res = eval(program);
  EXPECT_EQ(program->type(), res->type());
  EXPECT_STREQ(program->toString(), res->toString());
}

TEST_F(InterpreterTest, minimalAdd) {
  load("(+ 1 1)");
  const auto res = eval(program);
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(2, i->data());
}

TEST_F(InterpreterTest, addIdentity) {
  load("(+ 0)");
  const auto res = eval(program);
  ASSERT_EQ(2, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(0, i->data());
}

TEST_F(InterpreterTest, addMany) {
  load("(+ 1 1 1 1 1 1 1)");
  const auto res = eval(program);
  ASSERT_EQ(8, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(7, i->data());
}

TEST_F(InterpreterTest, minimalSub) {
  load("(- 2 1)");
  const auto res = eval(program);
  ASSERT_NE(nullptr, res);
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(1, i->data());
}

TEST_F(InterpreterTest, subMany) {
  load("(- 6 1 1 1 1 1 1)");
  const auto res = eval(program);
  ASSERT_EQ(8, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(0, i->data());
}

TEST_F(InterpreterTest, oneLevelDown) {
  load("(+ (+ 1 1) (- 1 1))");
  const auto res = eval(program);
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(2, i->data());
}

TEST_F(InterpreterTest, oneLevelDown2) {
  load("(+ (+ 1 1) 2)");
  const auto res = eval(program);
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(4, i->data());
}

TEST_F(InterpreterTest, minimalMul) {
  load("(* 2 3)");
  const auto res = eval(program);
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(6, i->data());
}

TEST_F(InterpreterTest, numericMustHaveArg) {
  try {
    load("(+)");
    ASSERT_NE(nullptr, program);
    eval(program);
    FAIL();
  } catch (SyntaxError &e) {
    const auto msg = "Expected operators for operator";
    EXPECT_EQ(0, strncmp(msg, e.what(), strlen(msg)));
  } catch (std::exception &e) {
    FAIL() << e.what();
  }
}

TEST_F(InterpreterTest, builtinIntOpMustHaveIntArgs) {
  try {
    load("(+ \"hest\")");
    ASSERT_NE(nullptr, program);
    eval(program);
    FAIL();
  } catch (SyntaxError &e) {
    const auto msg = "Operator works only on integer types";
    EXPECT_EQ(0, strncmp(msg, e.what(), strlen(msg)));
  } catch (std::exception &e) {
    FAIL() << e.what();
  }
}

TEST_F(InterpreterTest, mulMany) {
  load("(* 1 1 1 1 1 1 1)");
  const auto res = eval(program);
  ASSERT_EQ(8, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(1, i->data());
}

TEST_F(InterpreterTest, minimalDiv) {
  load("(/ 4 2)");
  const auto res = eval(program);
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(2, i->data());
}

TEST_F(InterpreterTest, divSome) {
  load("(/ 8 4 2)");
  const auto res = eval(program);
  ASSERT_EQ(4, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(1, i->data());
}

TEST_F(InterpreterTest, minimalMod) {
  load("(% 22 10)");
  const auto res = eval(program);
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(2, i->data());
}

TEST_F(InterpreterTest, modSome) {
  load("(% 13 4 2)");
  const auto res = eval(program);
  ASSERT_EQ(4, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(1, i->data());
}

TEST_F(InterpreterTest, listBuiltin) {
  load("(list a b c)");
  const auto res = eval(program);
  ASSERT_NE(nullptr, res);
  EXPECT_EQ(res->type(), AST::Type::BUILTIN);
  EXPECT_TRUE(res->isBuiltin(Builtin::LIST));
  EXPECT_EQ(res->children().size(), 3);
  EXPECT_EQ(res->head()->type(), AST::Type::SYMBOL);
}

TEST_F(InterpreterTest, emptyList) {
  load("(list)");
  const auto res = eval(program);
  ASSERT_NE(nullptr, res);
  EXPECT_EQ(res->type(), AST::Type::BUILTIN);
  EXPECT_TRUE(res->isBuiltin(Builtin::LIST));
  EXPECT_EQ(res->children().size(), 0);
}

TEST_F(InterpreterTest, head) {
  load("(head (list a))");
  const auto res = eval(program);
  ASSERT_NE(nullptr, res);
  EXPECT_EQ(res->type(), AST::Type::SYMBOL);
}

TEST_F(InterpreterTest, headMultipleArgs) {
  load("(head (list a) (list b))");
  try {
    eval(program);
    FAIL();
  } catch (SyntaxError &e) {
  }
}

TEST_F(InterpreterTest, headOnlyOnList) {
  load("(head 123)");
  try {
    eval(program);
    FAIL();
  } catch (SyntaxError &e) {
  }
}

TEST_F(InterpreterTest, headNotOnEmptyList) {
  load("(head (list))");
  try {
    eval(program);
    FAIL();
  } catch (SyntaxError &e) {
  }
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
