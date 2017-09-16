#include "Interpreter.h"
#include "SyntaxError.h"
#include "Util.h"

#include <cassert>
#include <functional>
#include <iostream>

static std::shared_ptr<AST> evalIntOp(Builtin intOp, AST::List ls);
static std::shared_ptr<AST> evalBuiltin(AST::List ls);
static void requireIntType(std::shared_ptr<AST> node);
static bool isIntOp(Builtin op);

static std::shared_ptr<AST> getSingleListArg(const std::shared_ptr<AST> &opnode,
                                             const AST::List &ls);

static void requireSingleArgument(const std::shared_ptr<AST> &opnode,
                                  AST::List ls);

static void requireListType(const std::shared_ptr<AST> &opnode,
                            std::shared_ptr<AST> node);
static void requireNonEmptyList(const std::shared_ptr<AST> &opnode,
                                std::shared_ptr<AST> node);

static int applyIntOp(const int acc, const AST::List ls, const size_t idx,
                      std::function<int(const int, const int)> op);

std::shared_ptr<AST> eval(std::shared_ptr<AST> node) {
  std::cout << "EVAL: ";
  util::print(node);
  std::cout << std::endl;

  if (node->children().size() == 0)
    return node;
  const auto op = eval(node->head());
  if (op->type() == AST::Type::BUILTIN) {
    return evalBuiltin(node->children());
  }
  std::cout << AST::TypeToCString(node->type()) << std::endl;
  throw SyntaxError("Uknown node type", node);
  return nullptr;
}

std::shared_ptr<AST> evalBuiltin(AST::List ls) {
  if (ls.front()->type() != AST::Type::BUILTIN) {
    throw SyntaxError("Expected builtin", ls.front());
  }
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
    return getSingleListArg(opnode, ls)->children().front();
  } else if (op == Builtin::TAIL) {
    return getSingleListArg(opnode, ls)->children().back();
  } else if (op == Builtin::JOIN) {
    AST::List children;
    for (auto it = (ls.begin() + 1); it != ls.end(); ++it) {
      const auto node = eval(*it);
      requireListType(opnode, node);
      const auto xs = node->children();
      children.insert(children.cend(), xs.cbegin(), xs.cend());
    }
    auto ret = std::make_shared<ASTBuiltin>(Builtin::LIST);
    ret->setChildren(children);
    return ret;
  } else if (op == Builtin::EVAL) {
    requireSingleArgument(opnode, ls);
    const auto node = eval(ls[1]);
    return eval(node);
  }

  return nullptr;
}

std::shared_ptr<AST> evalIntOp(Builtin intOp, AST::List ls) {
  if (ls.size() == 1)
    throw SyntaxError("Expected operators for operator", ls.front());

  const auto first = eval(ls[1]);
  const auto firstVal = std::static_pointer_cast<ASTInt>(first)->data();
  requireIntType(first);
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

int applyIntOp(const int acc, const AST::List ls, const size_t idx,
               std::function<int(const int, const int)> op) {
  if (ls.size() == idx)
    return acc;
  const auto n = eval(ls[idx]);
  requireIntType(n);
  const auto intNode = std::static_pointer_cast<ASTInt>(n);
  return applyIntOp(op(acc, intNode->data()), ls, idx + 1, op);
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

void requireIntType(std::shared_ptr<AST> node) {
  if (node->type() != AST::Type::INTEGER) {
    throw SyntaxError("Operator works only on integer types", node);
  }
}

std::shared_ptr<AST> getSingleListArg(const std::shared_ptr<AST> &opnode,
                                      const AST::List &ls) {
  requireSingleArgument(opnode, ls);
  const auto node = eval(ls[1]);
  requireNonEmptyList(opnode, node);
  return node;
}

void requireNonEmptyList(const std::shared_ptr<AST> &opnode,
                         std::shared_ptr<AST> node) {
  requireListType(opnode, node);
  if (0 == node->children().size())
    throw SyntaxError("Operator does not work on empty list", opnode);
}

void requireSingleArgument(const std::shared_ptr<AST> &opnode, AST::List ls) {
  if (2 != ls.size()) {
    throw SyntaxError("Builtin requires exactly one argument", opnode);
  }
}

void requireListType(const std::shared_ptr<AST> &opnode,
                     std::shared_ptr<AST> node) {
  if (!node || !node->isBuiltin(Builtin::LIST))
    throw SyntaxError("Argument must be a list", opnode);
}
