#include "Util.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

namespace util {
std::vector<Lexer::TokenType> tokenizeFile(std::string file) {
  std::ifstream in(file);
  std::stringstream buffer;
  buffer << in.rdbuf();

  std::string contents(buffer.str());
  std::vector<Lexer::TokenType> res;

  Lexer lexer(contents.c_str());
  do {
    res.emplace_back(lexer.nextToken());
  } while (res.back() != Lexer::TokenType::TOKEN_EOF);

  return res;
}

void print(std::shared_ptr<AST> node) {
  assert(node);
  using namespace std;
  using Type = AST::Type;
  switch (node->type()) {
  case Type::SEXPR: {
    cout << "(";
    bool first = true;
    for (const auto &child : node->children()) {
      if (!first) {
        cout << " ";
      } else {
        first = false;
      }
      print(child);
    }
    cout << ")";
    break;
  }
  case Type::SYMBOL:
  case Type::INTEGER:
  case Type::STRING:
  case Type::BUILTIN:
    cout << node->toString();
    break;
  }
}

} // util

namespace std {

ostream &operator<<(ostream &os, const Lexer::TokenType &t) {
  return os << Lexer::TokenToCString(t);
}

ostream &operator<<(ostream &os, const AST::Type &t) {
  return os << AST::TypeToCString(t);
}

} // std
