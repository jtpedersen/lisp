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

void print(const std::shared_ptr<AST> &node) {
  assert(node);
  using namespace std;
  using Type = AST::Type;
  switch (node->type()) {
  case Type::SEXPR: {
    cout << "(";
    printList(node->children());
    cout << ")";
    break;
  }
  case Type::SYMBOL:
  case Type::BOOLEAN:
  case Type::INTEGER:
  case Type::STRING:
  case Type::BUILTIN:
    cout << node->toString();
    break;
  case Type::FUN:
    cout << "<function>: ";
    printList(node->children());
    break;
  }
}

void printList(const AST::List &ls) {
  using namespace std;

  bool first = true;
  for (const auto &child : ls) {
    if (!first) {
      cout << " ";
    } else {
      first = false;
    }
    print(child);
  }
}

std::string zap_to_keyword(const std::string &data, const int index) {
  const auto idx = [&] {
    const auto start = std::max(0, index - 1);
    const auto end = static_cast<int>(data.length());
    for (int i = start; i < end; i++) {
      const auto & c = data[i] ;
      if ( c== '(' || std::isspace(c))
        return i + 1;
    }
    return start;
  }();
  return data.substr(idx);
}

} // namespace util

namespace std {

ostream &operator<<(ostream &os, const Lexer::TokenType &t) {
  return os << Lexer::TokenToCString(t);
}

ostream &operator<<(ostream &os, const AST::Type &t) {
  return os << AST::TypeToCString(t);
}

} // namespace std
