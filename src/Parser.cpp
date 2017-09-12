#include "Parser.h"
#include "SyntaxError.h"
#include "Util.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <string.h>

using TokenType = Lexer::TokenType;

Parser::Parser(const Lexer &lexer) : lexer(lexer) {}

std::shared_ptr<AST> Parser::read() { return readExpr(); }

std::shared_ptr<AST> Parser::readSexpr() {
  AST::List ls;
  while (const auto element = readExpr()) {
    //    std::cout << "Adding element: " << element->type() << std::endl;
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
    if (strcmp(symbolNode->data(), "define") == 0) {
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
  //  std::cout << "Read token: " << tokenType << std::endl;
  if (TokenType::SYMBOL == tokenType) {
    const auto name = lexer.string();
    const auto op = ASTBuiltin::operatorFromCString(name);
    if (ASTBuiltin::Operator::UNKNOWN != op) {
      return std::make_shared<ASTBuiltin>(op);
    }
    return std::make_shared<ASTSymbol>(name);
  } else if (TokenType::INTEGER == tokenType) {
    return std::make_shared<ASTInt>(lexer.integer());
  } else if (TokenType::STRING == tokenType) {
    return std::make_shared<ASTString>(lexer.string());
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
