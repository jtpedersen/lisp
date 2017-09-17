#include "Environment.h"
#include "Util.h"

#include <cassert>
#include <iostream>

Environment::Environment(std::shared_ptr<Environment> parent)
    : parent(parent) {}

Environment::~Environment() {
  for (const auto e : env) {
    free(const_cast<char *>(e.first));
  }
}

void Environment::setEntry(const char *symbol, std::shared_ptr<AST> node) {
  std::cout << "Set:" << symbol << "-->";
  util::print(node);
  std::cout <<std::endl;
  env[strdup(symbol)] = node;
  assert(env.find(symbol) != env.cend());
}

std::shared_ptr<AST> Environment::operator[](const char *symbol) {
  auto it = env.find(symbol);
  if (it != env.end())
    return (*it).second;
  if (parent)
    return (*parent)[symbol];
  return nullptr;
}
