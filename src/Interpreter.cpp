#include "Interpreter.h"
#include "Environment.h"
#include "SyntaxError.h"
#include "Util.h"

#include <cassert>
#include <functional>
#include <iostream>

Interpreter::Interpreter() : env(std::make_shared<Environment>()) {}

static void showNode(const char *prefix, const std::shared_ptr<AST> &node) {
#if 0
  std::cout << prefix;
  util::print(node);
  std::cout << std::endl;
#endif
}

std::shared_ptr<AST> Interpreter::eval(std::shared_ptr<AST> node) {
  showNode("EVAL: ", node);
  if (node->children().size() == 0) {
    if (node->type() == AST::Type::SYMBOL) {
      const auto &symbol = std::static_pointer_cast<ASTSymbol>(node);
      const auto name = symbol->data();
      const auto ret = (*env)[name];
      if (!ret) {
        std::cout << "name:" << name << std::endl;
        throw SyntaxError("Identifier is not known:", node);
      }
      return ret;
    }
    return node;
  }

  const auto op = eval(node->head());
  if (op->type() == AST::Type::BUILTIN) {
    return evalBuiltin(node);
  } else if (op->type() == AST::Type::FUN) {
    assert(op->children().size() == 3);
    const auto arglist = op->children()[1];
    showNode("ArgList: ", arglist);

    showNode("Call: ", node);

    for (unsigned int i = 1; i < arglist->children().size(); i++) {
      env->setEntry(symbolName(arglist->children()[i]),
                    eval(node->children()[i]));
    }
    return eval(op->children()[2]);
  }
  std::cout << AST::TypeToCString(node->type()) << std::endl;
  throw SyntaxError("Uknown node type", node);
  return nullptr;
}

std::shared_ptr<AST> Interpreter::evalBuiltin(std::shared_ptr<AST> node) {
  AST::List ls = node->children();
  if (ls.front()->type() != AST::Type::BUILTIN) {
    throw SyntaxError("Expected builtin", ls.front());
  }
  const auto opnode = std::static_pointer_cast<ASTBuiltin>(ls.front());
  const auto op = opnode->op();

  switch (op) {
  case Builtin::ADD:
  case Builtin::SUB:
  case Builtin::DIV:
  case Builtin::MUL:
  case Builtin::MOD:
    return evalIntOp(op, ls);
  case Builtin::DEFINE:
    return evalDefine(node);
  case Builtin::LIST:
    return evalList(ls);
  case Builtin::HEAD:
    return getSingleListArg(opnode, ls)->children().front();
  case Builtin::TAIL:
    return getSingleListArg(opnode, ls)->children().back();
  case Builtin::JOIN:
    return evalJoin(opnode, ls);
  case Builtin::EVAL:
    return eval(getSingleListArg(opnode, ls));
  case Builtin::PPRINT:
    return evalPPrint(opnode, ls);
  case Builtin::IF:
    return evalIf(ls);
  case Builtin::UNKNOWN:
    break;
  };
  throw SyntaxError("Unimplemented builtin", opnode);
  return nullptr;
}

std::shared_ptr<AST> Interpreter::evalList(const AST::List &ls) {
  auto ret = std::make_shared<ASTBuiltin>(Builtin::LIST);
  AST::List children(ls.begin() + 1, ls.end());
  ret->setChildren(children);
  return ret;
}

std::shared_ptr<AST> Interpreter::evalJoin(const std::shared_ptr<AST> &opnode,
                                           const AST::List &ls) {
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
}

std::shared_ptr<AST> Interpreter::evalIf(const AST::List &ls) {
  const auto res = eval(ls[1]);
  if (0 != std::static_pointer_cast<ASTInt>(res)->data()) {
    return eval(ls[2]);
  } else {
    return eval(ls[3]);
  }
}

std::shared_ptr<AST> Interpreter::evalPPrint(const std::shared_ptr<AST> &opnode,
                                             const AST::List &ls) {
  requireSingleArgument(opnode, ls);
  const auto node = ls[1];
  util::print(node);
  std::cout << std::endl;
  return eval(node);
}

std::shared_ptr<AST> Interpreter::evalIntOp(Builtin intOp, AST::List ls) {
  if (ls.size() == 1)
    throw SyntaxError("Expected operators for operator", ls.front());

  const auto first = eval(ls[1]);
  requireIntType(first);
  const auto firstVal = std::static_pointer_cast<ASTInt>(first)->data();

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

int Interpreter::applyIntOp(const int acc, const AST::List ls, const size_t idx,
                            std::function<int(const int, const int)> op) {
  if (ls.size() == idx)
    return acc;
  const auto n = eval(ls[idx]);
  requireIntType(n);
  const auto intNode = std::static_pointer_cast<ASTInt>(n);
  return applyIntOp(op(acc, intNode->data()), ls, idx + 1, op);
}

std::shared_ptr<AST> Interpreter::evalDefine(const std::shared_ptr<AST> &node) {
  const auto ls = node->children();
  const auto argList = ls[1];
  if (AST::Type::SYMBOL == argList->type()) {
    // Define a variable
    const auto name = symbolName(argList);
    const auto body = ls[2];
    env->setEntry(name, body);
  } else {
    showNode("Define function: ", node);
    if (argList->children().size() < 1) {
      throw SyntaxError("Define must have a name", node);
    }
    for (const auto &child : argList->children()) {
      if (AST::Type::SYMBOL != child->type()) {
        throw SyntaxError("Arglist must only contain identifiers", argList);
      }
    }
    node->setType(AST::Type::FUN);
    env->setEntry(symbolName(argList->head()), node);
  }
  return std::make_shared<ASTSexpr>();
}

void Interpreter::requireIntType(std::shared_ptr<AST> node) {
  if (node->type() != AST::Type::INTEGER) {
    throw SyntaxError("Operator works only on integer types", node);
  }
}

std::shared_ptr<AST>
Interpreter::getSingleListArg(const std::shared_ptr<AST> &opnode,
                              const AST::List &ls) {
  requireSingleArgument(opnode, ls);
  const auto node = eval(ls[1]);
  requireNonEmptyList(opnode, node);
  return node;
}

void Interpreter::requireNonEmptyList(const std::shared_ptr<AST> &opnode,
                                      std::shared_ptr<AST> node) {
  requireListType(opnode, node);
  if (0 == node->children().size())
    throw SyntaxError("Operator does not work on empty list", opnode);
}

void Interpreter::requireSingleArgument(const std::shared_ptr<AST> &opnode,
                                        AST::List ls) {
  if (2 != ls.size()) {
    throw SyntaxError("Builtin requires exactly one argument", opnode);
  }
}

void Interpreter::requireListType(const std::shared_ptr<AST> &opnode,
                                  std::shared_ptr<AST> node) {
  if (!node || !node->isBuiltin(Builtin::LIST))
    throw SyntaxError("Argument must be a list", opnode);
}

const char *Interpreter::symbolName(std::shared_ptr<AST> node) {
  if (AST::Type::SYMBOL != node->type())
    throw SyntaxError("Expected symbol", node);
  return std::static_pointer_cast<ASTSymbol>(node)->data();
}
