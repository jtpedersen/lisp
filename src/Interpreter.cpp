#include "Interpreter.h"
#include "SyntaxError.h"
#include <cassert>
#include <functional>
#include <iostream>

int applyIntOp(const int acc, const AST::List ls, const size_t idx,
               std::function<int(const int, const int)> op) {

  if (ls.size() == idx)
    return acc;
  assert(ls[idx]->type() == AST::Type::INTEGER);
  const auto intNode = std::static_pointer_cast<ASTInt>(ls[idx]);
  return applyIntOp(op(acc, intNode->data()), ls, idx + 1, op);
}

std::shared_ptr<AST> evalBuiltin(AST::List ls) {
  assert(ls.front()->type() == AST::Type::BUILTIN);
  //  std::cout << "EVAL BUILTIN: " << ls.front()->toString() << std::endl;
  if (ls.size() == 1)
    throw SyntaxError("Expected operators for operator", ls.front());

  const auto opnode = std::static_pointer_cast<ASTBuiltin>(ls.front());

  if (ls[1]->type() != AST::Type::INTEGER) {
    throw SyntaxError("Operator works only on integer types", ls.front());
  }

  const auto firstVal = std::static_pointer_cast<ASTInt>(ls[1])->data();

  const auto op = [opType = opnode->op()]()
                      ->std::function<int(const int, const int)> {
    switch (opType) {
    case ASTBuiltin::Operator::ADD:
      return [](const int a, const int b) { return a + b; };
    case ASTBuiltin::Operator::SUB:
      return [](const int a, const int b) { return a - b; };
    case ASTBuiltin::Operator::MUL:
      return [](const int a, const int b) { return a * b; };
    case ASTBuiltin::Operator::DIV:
      return [](const int a, const int b) { return a / b; };
    case ASTBuiltin::Operator::MOD:
      return [](const int a, const int b) { return a % b; };
    case ASTBuiltin::Operator::UNKNOWN:
      throw new SyntaxError("Can not use unknown op", 0, 0);
    }
    return nullptr;
  }
  ();
  return std::make_shared<ASTInt>(applyIntOp(firstVal, ls, 2, op));
}

std::shared_ptr<AST> eval(std::shared_ptr<AST> node) {
  // std::cout << "EVAL: " << node->toString()
  //           << "childCnt:" << node->children().size() << std::endl;
  if (node->children().size() == 0)
    return node;
  AST::List ls;
  for (const auto &child : node->children()) {
    ls.emplace_back(eval(child));
  }

  if (ls.front()->type() == AST::Type::BUILTIN) {
    return evalBuiltin(ls);
  }
  // TODO: syntax error
  return nullptr;
}
