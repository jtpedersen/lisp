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
  if (AST::Type::BUILTIN == first->type()) {
    checkSyntaxForBuiltin(ls);
  }
  return ret;
}

std::shared_ptr<AST> Parser::readExpr() {
  const auto tokenType = lexer.nextToken();
  //  std::cout << "Read token: " << tokenType << std::endl;
  if (TokenType::SYMBOL == tokenType) {
    const auto name = lexer.string();
    const auto op = builtinFromCString(name);
    if (Builtin::UNKNOWN != op) {
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

void Parser::checkSyntaxForBuiltin(AST::List ls) const {
  const auto first = std::static_pointer_cast<ASTBuiltin>(ls.front());
  if (!first)
    throw SyntaxError("Expected builtin", ls.front());
  switch (first->op()) {
  case Builtin::ADD:
  case Builtin::SUB:
  case Builtin::DIV:
  case Builtin::MUL:
  case Builtin::MOD:
  case Builtin::HEAD:
  case Builtin::TAIL:
    if (1 == ls.size()) {
      throw SyntaxError("Builtin requires operands", first);
    }
    break;

    case Builtin::DEFINE: {
    if (1 == ls.size()) {
      throw SyntaxError("Define must have argumentlist and body", first);
    } else if (2 == ls.size()) {
      throw SyntaxError("Define must have a body", first);
    }
    break;
  }
  case Builtin::LIST:
    break;
  case Builtin::UNKNOWN:
    break;
  }
}

void Parser::syntaxError(const char *msg) {
  throw SyntaxError(msg, lexer.line(), lexer.col());
}
