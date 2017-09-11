#include "Interpreter.h"

#include <cassert>
#include <functional>
#include <iostream>

int applyIntOp(const int acc, const AST::List ls, const size_t idx,
               std::function<int(const int, const int)> op) {

  if (ls.size() == idx)
    return acc;
  std::cout << acc << " for " << idx << std::endl;
  assert(ls[idx]->type() == AST::Type::INTEGER);
  const auto intNode = std::static_pointer_cast<ASTInt>(ls[idx]);
  return applyIntOp(op(acc, intNode->data()), ls, idx + 1, op);
}

std::shared_ptr<AST> evalBuiltin(AST::List ls) {
  assert(ls.front()->type() == AST::Type::BUILTIN);
  std::cout << "EVAL BUILTIN: " << ls.front()->toString() << std::endl;
  const auto opnode = std::static_pointer_cast<ASTBuiltin>(ls.front());
  switch (opnode->op()) {
  case ASTBuiltin::Operator::ADD:
    return std::make_shared<ASTInt>(
        applyIntOp(0, ls, 1, [](const int a, const int b) { return a + b; }));
  case ASTBuiltin::Operator::SUB: {
    if (ls.size() == 1)
      return std::make_shared<ASTInt>(0);
    assert(ls[1]->type() == AST::Type::INTEGER);
    const auto intNode = std::static_pointer_cast<ASTInt>(ls[1]);

    return std::make_shared<ASTInt>(
        applyIntOp(intNode->data(), ls, 2,
                   [](const int a, const int b) { return a - b; }));
  }
  }
  return nullptr;
}

std::shared_ptr<AST> eval(std::shared_ptr<AST> node) {
  std::cout << "EVAL: " << node->toString()
            << "childCnt:" << node->children().size() << std::endl;
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
