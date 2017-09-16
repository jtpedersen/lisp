#include "Interpreter.h"
#include "SyntaxError.h"
#include <cassert>
#include <functional>
#include <iostream>

static std::shared_ptr<AST> evalIntOp(Builtin intOp, AST::List ls);
static bool isIntOp(Builtin op);
static std::shared_ptr<AST> evalBuiltin(AST::List ls);

int applyIntOp(const int acc, const AST::List ls, const size_t idx,
               std::function<int(const int, const int)> op) {
  if (ls.size() == idx)
    return acc;
  assert(ls[idx]->type() == AST::Type::INTEGER);
  const auto intNode = std::static_pointer_cast<ASTInt>(ls[idx]);
  return applyIntOp(op(acc, intNode->data()), ls, idx + 1, op);
}

std::shared_ptr<AST> eval(std::shared_ptr<AST> node) {
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

std::shared_ptr<AST> evalBuiltin(AST::List ls) {
  assert(ls.front()->type() == AST::Type::BUILTIN);
  const auto opnode = std::static_pointer_cast<ASTBuiltin>(ls.front());
  const auto op = opnode->op();
  if (isIntOp(op)) {
    return evalIntOp(op, ls);
  }
  if (op == Builtin::LIST) {
    auto ret = std::make_shared<ASTBuiltin>(Builtin::LIST);
    AST::List children(ls.begin() + 1, ls.end());
    ret->setChildren(children);
    return ret;
  } else if (op == Builtin::HEAD) {
    if (2 != ls.size()) {
      throw SyntaxError("Head requires exactly one argument", opnode);
    }

    if (!ls[1]->isBuiltin(Builtin::LIST))
      throw SyntaxError("Head only works on lists", opnode);
    if (0 == ls[1]->children().size())
      throw SyntaxError("Head on empty list", opnode);
    return eval(ls[1]->children()[0]);
  }
  return nullptr;
}

std::shared_ptr<AST> evalIntOp(Builtin intOp, AST::List ls) {
  if (ls.size() == 1)
    throw SyntaxError("Expected operators for operator", ls.front());

  const auto first = ls[1];
  const auto firstVal = std::static_pointer_cast<ASTInt>(first)->data();
  if (first->type() != AST::Type::INTEGER) {
    throw SyntaxError("Operator works only on integer types", ls.front());
  }
  const auto op = [intOp, &ls]() -> std::function<int(const int, const int)> {
    switch (intOp) {
    case Builtin::ADD:
      return [](const int a, const int b) { return a + b; };
    case Builtin::SUB:
      return [](const int a, const int b) { return a - b; };
    case Builtin::MUL:
      return [](const int a, const int b) { return a * b; };
    case Builtin::DIV:
      return [](const int a, const int b) { return a / b; };
    case Builtin::MOD:
      return [](const int a, const int b) { return a % b; };
    default:
      throw SyntaxError("Can not use unknown op", ls.front());
    }
    return nullptr;
  }();
  return std::make_shared<ASTInt>(applyIntOp(firstVal, ls, 2, op));
}

bool isIntOp(Builtin op) {
  switch (op) {
  case Builtin::ADD:
  case Builtin::SUB:
  case Builtin::MUL:
  case Builtin::DIV:
  case Builtin::MOD:
    return true;
  default:
    return false;
  }
}
