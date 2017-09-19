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

  std::shared_ptr<AST> eval() { return interpreter.eval(program); }
  std::shared_ptr<AST> program;
  Interpreter interpreter;
};

TEST_F(InterpreterTest, minimal) {
  load("12");
  const auto res = eval();
  EXPECT_EQ(program->type(), res->type());
  EXPECT_STREQ(program->toString(), res->toString());
}

TEST_F(InterpreterTest, minimalEmpty) {
  load("()");
  const auto res = eval();
  EXPECT_EQ(program->type(), res->type());
  EXPECT_STREQ(program->toString(), res->toString());
}

TEST_F(InterpreterTest, minimalAdd) {
  load("(+ 1 1)");
  const auto res = eval();
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(2, i->data());
}

TEST_F(InterpreterTest, addIdentity) {
  load("(+ 0)");
  const auto res = eval();
  ASSERT_EQ(2, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(0, i->data());
}

TEST_F(InterpreterTest, addMany) {
  load("(+ 1 1 1 1 1 1 1)");
  const auto res = eval();
  ASSERT_EQ(8, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(7, i->data());
}

TEST_F(InterpreterTest, minimalSub) {
  load("(- 2 1)");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(1, i->data());
}

TEST_F(InterpreterTest, subMany) {
  load("(- 6 1 1 1 1 1 1)");
  const auto res = eval();
  ASSERT_EQ(8, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(0, i->data());
}

TEST_F(InterpreterTest, oneLevelDown) {
  load("(+ (+ 1 1) (- 1 1))");
  const auto res = eval();
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(2, i->data());
}

TEST_F(InterpreterTest, oneLevelDown2) {
  load("(+ (+ 1 1) 2)");
  const auto res = eval();
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(4, i->data());
}

TEST_F(InterpreterTest, minimalMul) {
  load("(* 2 3)");
  const auto res = eval();
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(6, i->data());
}

TEST_F(InterpreterTest, numericMustHaveArg) {
  try {
    load("(+)");
    ASSERT_NE(nullptr, program);
    eval();
    FAIL();
  } catch (SyntaxError &e) {
    const auto msg = "Builtin requires operands";
    EXPECT_EQ(0, strncmp(msg, e.what(), strlen(msg)));
  } catch (std::exception &e) {
    FAIL() << e.what();
  }
}

TEST_F(InterpreterTest, builtinIntOpMustHaveIntArgs) {
  try {
    load("(+ \"hest\")");
    ASSERT_NE(nullptr, program);
    eval();
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
  const auto res = eval();
  ASSERT_EQ(8, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(1, i->data());
}

TEST_F(InterpreterTest, minimalDiv) {
  load("(/ 4 2)");
  const auto res = eval();
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(2, i->data());
}

TEST_F(InterpreterTest, divSome) {
  load("(/ 8 4 2)");
  const auto res = eval();
  ASSERT_EQ(4, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(1, i->data());
}

TEST_F(InterpreterTest, minimalMod) {
  load("(% 22 10)");
  const auto res = eval();
  ASSERT_EQ(3, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(2, i->data());
}

TEST_F(InterpreterTest, modSome) {
  load("(% 13 4 2)");
  const auto res = eval();
  ASSERT_EQ(4, program->children().size());
  ASSERT_EQ(res->type(), AST::Type::INTEGER);
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(1, i->data());
}

TEST_F(InterpreterTest, listBuiltin) {
  load("(list a b c)");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  EXPECT_EQ(res->type(), AST::Type::BUILTIN);
  EXPECT_TRUE(res->isBuiltin(Builtin::LIST));
  EXPECT_EQ(res->children().size(), 3);
  EXPECT_EQ(res->head()->type(), AST::Type::SYMBOL);
}

TEST_F(InterpreterTest, emptyList) {
  load("(list)");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  EXPECT_EQ(res->type(), AST::Type::BUILTIN);
  EXPECT_TRUE(res->isBuiltin(Builtin::LIST));
  EXPECT_EQ(res->children().size(), 0);
}

TEST_F(InterpreterTest, head) {
  load("(head (list a))");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  EXPECT_EQ(res->type(), AST::Type::SYMBOL);
}

TEST_F(InterpreterTest, headMultipleArgs) {
  load("(head (list a) (list b))");
  try {
    eval();
    FAIL();
  } catch (SyntaxError &e) {
  }
}

TEST_F(InterpreterTest, headOnlyOnList) {
  load("(head 123)");
  try {
    eval();
    FAIL();
  } catch (SyntaxError &e) {
  }
}

TEST_F(InterpreterTest, headNotOnEmptyList) {
  load("(head (list))");
  try {
    eval();
    FAIL();
  } catch (SyntaxError &e) {
  }
}

TEST_F(InterpreterTest, tail) {
  load("(tail (list a))");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  EXPECT_EQ(res->type(), AST::Type::SYMBOL);
}

TEST_F(InterpreterTest, tailMultipleArgs) {
  load("(tail (list a) (list b))");
  try {
    eval();
    FAIL();
  } catch (SyntaxError &e) {
  }
}

TEST_F(InterpreterTest, tailOnlyOnList) {
  load("(tail 123)");
  try {
    eval();
    FAIL();
  } catch (SyntaxError &e) {
  }
}

TEST_F(InterpreterTest, tailNotOnEmptyList) {
  load("(tail (list))");
  try {
    eval();
    FAIL();
  } catch (SyntaxError &e) {
  }
}

TEST_F(InterpreterTest, joinBuiltin) {
  load("(join (list a) (list b))");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  EXPECT_TRUE(res->isBuiltin(Builtin::LIST)) << res->toString();
  EXPECT_EQ(2, res->children().size()) << res->toString();
}

TEST_F(InterpreterTest, joinRequiresAtLeastTwo) {
  try {
    load("(join (list))");
    eval();
    FAIL();
  } catch (SyntaxError &e) {
  }
}

TEST_F(InterpreterTest, joinManyLists) {
  load("(join (list a) (list b) (list c) (list d))");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  EXPECT_TRUE(res->isBuiltin(Builtin::LIST)) << res->toString();
  EXPECT_EQ(4, res->children().size()) << res->toString();
}

TEST_F(InterpreterTest, evalBuiltin) {
  load("(eval (list + 1 2))");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  ASSERT_EQ(res->type(), AST::Type::INTEGER) << res->toString();
  EXPECT_EQ(0, res->children().size()) << res->toString();
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(3, i->data());
}

TEST_F(InterpreterTest, evalMultiArgs) {
  load("(eval (list - 1 1) \"hest\")");
  try {
    eval();
    FAIL();
  } catch (SyntaxError &e) {
  }
}

TEST_F(InterpreterTest, evalNotOnAtom) {
  load("(eval \"hest\")");
  try {
    eval();
    FAIL();
  } catch (SyntaxError &e) {
  }
}

TEST_F(InterpreterTest, pprintAtom) {
  load("(pprint 123)");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  ASSERT_EQ(res->type(), AST::Type::INTEGER) << res->toString();
  EXPECT_EQ(0, res->children().size()) << res->toString();
  const auto i = std::static_pointer_cast<ASTInt>(res);
  EXPECT_EQ(123, i->data());
}

TEST_F(InterpreterTest, defineBuiltin) {
  load("(define x 2)");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  ASSERT_EQ(res->type(), AST::Type::SEXPR) << res->toString();
  EXPECT_EQ(0, res->children().size()) << res->toString();
}

TEST_F(InterpreterTest, defineAffectsEnv) {
  {
    load("(define x 2)");
    const auto res = eval();
    ASSERT_NE(nullptr, res);
    ASSERT_EQ(res->type(), AST::Type::SEXPR) << res->toString();
    EXPECT_EQ(0, res->children().size()) << res->toString();
  }
  {
    load("(+ x x)");
    const auto res = eval();
    ASSERT_NE(nullptr, res);
    EXPECT_EQ(0, res->children().size()) << res->toString();
    const auto i = std::static_pointer_cast<ASTInt>(res);
    EXPECT_EQ(4, i->data());
  }
}

TEST_F(InterpreterTest, defunBindsArgs) {
  load("(define (f y) (+ y 1))");
  auto res = eval();
  ASSERT_NE(nullptr, res);
  load("(f 2)");
  res = eval();
}

TEST_F(InterpreterTest, ifPrimitive) {
  load("(if 0 \"true\" \"false\")");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  const auto str = std::static_pointer_cast<ASTString>(res);
  EXPECT_STREQ(str->data(), "false");
}

TEST_F(InterpreterTest, ifPrimitiveTrue) {
  load("(if \"somethingThatIsNotIntegerZeroe\" \"true\" \"false\")");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  const auto str = std::static_pointer_cast<ASTString>(res);
  EXPECT_STREQ(str->data(), "true");
}

TEST_F(InterpreterTest, eqFalseString) {
  load("(= \"true\" \"false\")");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  ASSERT_EQ(res->type(), AST::Type::BOOLEAN);
  const auto str = std::static_pointer_cast<ASTBoolean>(res);
  EXPECT_FALSE(str->data());
}

TEST_F(InterpreterTest, eqTrueString) {
  load("(= \"true\" \"true\")");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  ASSERT_EQ(res->type(), AST::Type::BOOLEAN);
  const auto str = std::static_pointer_cast<ASTBoolean>(res);
  EXPECT_TRUE(str->data());
}

TEST_F(InterpreterTest, eqFalseInteger) {
  load("(= 0 0 0 1)");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  ASSERT_EQ(res->type(), AST::Type::BOOLEAN);
  const auto str = std::static_pointer_cast<ASTBoolean>(res);
  EXPECT_FALSE(str->data());
}

TEST_F(InterpreterTest, eqTrueInteger) {
  load("(= 2 2 2 2 2 2)");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  ASSERT_EQ(res->type(), AST::Type::BOOLEAN);
  const auto str = std::static_pointer_cast<ASTBoolean>(res);
  EXPECT_TRUE(str->data());
}

TEST_F(InterpreterTest, ltFalseInteger) {
  load("(< 4 3 2 1)");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  ASSERT_EQ(res->type(), AST::Type::BOOLEAN);
  const auto str = std::static_pointer_cast<ASTBoolean>(res);
  EXPECT_FALSE(str->data());
}

TEST_F(InterpreterTest, ltTrueInteger) {
  load("(< 1 2 3 4)");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  ASSERT_EQ(res->type(), AST::Type::BOOLEAN);
  const auto str = std::static_pointer_cast<ASTBoolean>(res);
  EXPECT_TRUE(str->data());
}

TEST_F(InterpreterTest, ltSameTypeArgs) {
  load("(< 1 2 3 4 hest)");
  try {
    const auto res = eval();
    FAIL() << res->toString();
  } catch (SyntaxError &e) {
  }
}

TEST_F(InterpreterTest, gtFalseInteger) {
  load("(> 1 2 3 4)");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  ASSERT_EQ(res->type(), AST::Type::BOOLEAN);
  const auto str = std::static_pointer_cast<ASTBoolean>(res);
  EXPECT_FALSE(str->data());
}

TEST_F(InterpreterTest, gtTrueInteger) {
  load("(> 4 3 2 1)");
  const auto res = eval();
  ASSERT_NE(nullptr, res);
  ASSERT_EQ(res->type(), AST::Type::BOOLEAN);
  const auto str = std::static_pointer_cast<ASTBoolean>(res);
  EXPECT_TRUE(str->data());
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
