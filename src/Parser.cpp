#include "Parser.h"
#include "Util.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <string.h>

using TokenType = Lexer::TokenType;

SyntaxError::SyntaxError(const char *what, int line, int col)
    : msg(what), line(line), col(col) {}

const char *SyntaxError::what() const noexcept {
  char *buf = static_cast<char *>(malloc(strlen(msg) + 1024));
  sprintf(buf, "%s on line %d:%d", msg, line, col);
  return buf;
}

Parser::Parser(const Lexer &lexer) : lexer(lexer) {}

std::shared_ptr<AST> Parser::read() {
  const auto tokenType = lexer.nextToken();
  if (TokenType::START_PAREN != tokenType) {
    syntaxError((std::string("Expected Sexpr start but got ") +
                 Lexer::TokenToCString(tokenType))
                    .c_str());
  }
  return readSexpr();
}

std::shared_ptr<AST> Parser::readSexpr() {
  AST::List ls;
  while (const auto element = readExpr()) {
    std::cout << "Adding element: " << element->type() << std::endl;
    ls.emplace_back(element);
  }
  if (ls.size() == 0) {
    return std::make_shared<ASTSexpr>();
  }
  const auto ret = std::make_shared<ASTSexpr>();
  ret->setChildren(ls);

  const auto first = ls[0];
  if (first->type() == AST::Type::SYMBOL) {
    auto symbolNode = std::static_pointer_cast<ASTSymbol>(first);
    if (strcmp(symbolNode->value(), "define") == 0) {
      ret->setType(AST::Type::DEFINE);
      if (1 == ls.size()) {
        syntaxError("Define must have argumentlist and body");
      } else if (2 == ls.size()) {
        syntaxError("Define must have a body");
      }
    }
  }
  return ret;
}

std::shared_ptr<AST> Parser::readExpr() {
  const auto tokenType = lexer.nextToken();
  std::cout << "Read token: " << tokenType << std::endl;
  if (TokenType::SYMBOL == tokenType) {
    const auto name = lexer.string();
    const auto op = ASTBuiltin::operatorFromCString(name);
    if (ASTBuiltin::Operator::UNKNOWN != op) {
      return std::make_shared<ASTBuiltin>(op);
    }
    return std::make_shared<ASTSymbol>(name);
  } else if (TokenType::INTEGER == tokenType) {
    return std::make_shared<ASTInt>(lexer.integer());
  } else if (TokenType::START_PAREN == tokenType) {
    return readSexpr();
  } else if (TokenType::END_PAREN == tokenType) {
    return nullptr;
  }
  syntaxError("Expected expression");
  return nullptr; // to please the compiler
}

void Parser::syntaxError(const char *msg) {
  throw SyntaxError(msg, lexer.line(), lexer.col());
}

bool Parser::isBuiltin(const char *name) const {
  std::cout << "is " << name << " a + " << strcmp(name, "+") << "   " << strlen(name) << std::endl;
  return 0 == strcmp("+", name);
}
std::shared_ptr<ASTBuiltin> Parser::builtinFromName(const char *name) {
  return nullptr;
}
