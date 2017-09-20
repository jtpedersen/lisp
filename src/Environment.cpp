#include "Environment.h"
#include "Util.h"

#include <cassert>
#include <iostream>

Environment::Environment(std::shared_ptr<Environment> parent)
    : parent_(parent) {}

Environment::~Environment() {
  for (const auto e : env) {
    free(const_cast<char *>(e.first));
  }
}

void Environment::setEntry(const char *symbol, std::shared_ptr<AST> node) {
  // std::cout << "Set:" << symbol << "-->";
  // util::print(node);
  // std::cout <<std::endl;
  env[strdup(symbol)] = node;
  assert(env.find(symbol) != env.cend());
}

std::shared_ptr<AST> Environment::operator[](const char *symbol) {
  auto it = env.find(symbol);
  if (it != env.end())
    return (*it).second;
  if (parent_)
    return (*parent_)[symbol];
  return nullptr;
}

std::shared_ptr<Environment> Environment::parent() const { return parent_; }

void Environment::setParent(std::shared_ptr<Environment> o) { parent_ = o; }

void Environment::dump() const {
  std::cout << "---------Environment dump-------------" << std::endl;
  for (const auto e : env) {
    std::cout << e.first << ": " << e.second->toString() << std::endl;
  }
  std::cout << "--------------------------------------" << std::endl;
}
