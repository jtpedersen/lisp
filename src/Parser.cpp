#include "Parser.h"
#include "Util.h"
#include <cassert>
#include <iostream>
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
  auto ret = std::make_shared<ASTSexpr>();
  while (const auto child = readExpr()) {
    std::cout << "Adding child: " << child->type() << std::endl;
    ret->addChild(child);
  }
  return ret;
}

std::shared_ptr<AST> Parser::readExpr() {
  const auto tokenType = lexer.nextToken();
  std::cout << "Read token: " << tokenType << std::endl;
  if (TokenType::SYMBOL == tokenType) {
    return std::make_shared<ASTSymbol>(lexer.string());
  } else if (TokenType::INTEGER == tokenType) {
    return std::make_shared<ASTInt>(lexer.integer());
  } else if (TokenType::START_PAREN == tokenType) {
    return readSexpr();
  } else if (TokenType::END_PAREN == tokenType) {
    return nullptr;
  }
  syntaxError("Expected expression");
  return nullptr;               // to please the compiler
}

void Parser::syntaxError(const char *msg) {
  throw SyntaxError(msg, lexer.line(), lexer.col());
}
