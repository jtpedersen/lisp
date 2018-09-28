#include "Interpreter.h"
#include "Environment.h"
#include "SyntaxError.h"
#include "Util.h"

#include <algorithm>
#include <cassert>
#include <iostream>

Interpreter::Interpreter() : env(std::make_shared<Environment>()) {}

static void showNode(const char *prefix, const std::shared_ptr<AST> &node) {
#if 0
  std::cout << prefix;
  if(node)
    util::print(node);
  std::cout << std::endl;
#endif
}

const std::shared_ptr<Environment> Interpreter::environment() const {
  return env;
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
    auto newEnv = std::make_shared<Environment>();
    for (unsigned int i = 1; i < arglist->children().size(); i++) {
      newEnv->setEntry(symbolName(arglist->children()[i]),
                       eval(node->children()[i]));
    }
    //    newEnv->dump();
    newEnv->setParent(env);
    env = newEnv;
    const auto res = eval(op->children()[2]);
    env = env->parent();
    return res;
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
  case Builtin::EQ:
    return evalEq(ls);
  case Builtin::GT:
    return evalIntCmp(ls, [](const auto &a, const auto &b) { return a > b; });
  case Builtin::GE:
    return evalIntCmp(ls, [](const auto &a, const auto &b) { return a >= b; });
  case Builtin::LT:
    return evalIntCmp(ls, [](const auto &a, const auto &b) { return a < b; });
  case Builtin::LE:
    return evalIntCmp(ls, [](const auto &a, const auto &b) { return a <= b; });
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
  const auto isTrue = [ type = res->type(), &res ] {
    if (AST::Type::BOOLEAN == res->type()) {
      return std::static_pointer_cast<ASTBoolean>(res)->data();
    }
    if (res->isBuiltin(Builtin::LIST)) {
      return res->children().size() > 0;
    }

    return 0 != std::static_pointer_cast<ASTInt>(res)->data();
  }
  ();

  if (isTrue) {
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

template <typename PtrType, class BinOp>
bool applyOp(const AST::List &ls, BinOp op) {
  for (unsigned int i = 1; i < ls.size(); i++) {
    const auto &a = std::static_pointer_cast<PtrType>(ls[i - 1])->data();
    const auto &b = std::static_pointer_cast<PtrType>(ls[i])->data();
    if (!op(a, b))
      return false;
  }
  return true;
}

std::shared_ptr<AST> Interpreter::evalEq(AST::List xs) {
  const auto ys = getEvaledArgs(xs);
  switch (ys.front()->type()) {
  case AST::Type::STRING: {
    const auto res = applyOp<ASTString>(
        ys, [](const auto &a, const auto &b) { return 0 == strcmp(a, b); });
    return std::make_shared<ASTBoolean>(res);
  }
  case AST::Type::INTEGER: {
    const auto res = applyOp<ASTInt>(
        ys, [](const auto &a, const auto &b) { return a == b; });
    return std::make_shared<ASTBoolean>(res);
  }
  default:
    throw SyntaxError("Uniplemented operation", xs.front());
  }

  return std::make_shared<ASTBoolean>(false);
}

std::shared_ptr<AST>
Interpreter::evalIntCmp(const AST::List &ls,
                        std::function<bool(const int &, const int &)> cmp) {
  const auto ys = getEvaledArgs(ls);
  requireIntType(ys.front());
  const auto res = applyOp<ASTInt>(ys, cmp);
  return std::make_shared<ASTBoolean>(res);
}

AST::List Interpreter::getEvaledArgs(const AST::List xs) {
  if (xs.size() < 3)
    throw SyntaxError("Expected operators for operator", xs.front());

  AST::List ys;
  for (unsigned int i = 1; i < xs.size(); i++) {
    ys.emplace_back(eval(xs[i]));
  }
  if (!std::all_of(std::cbegin(ys), std::cend(ys),
                   [type = ys.front()->type()](const auto &y) {
                     return type == y->type();
                   })) {
    throw SyntaxError("All arguments must be of same type", xs.front());
  }
  return ys;
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
