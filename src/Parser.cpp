#include "Parser.h"
#include <cassert>
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

std::shared_ptr<AST> Parser::read() { return readSexpr(); }

std::shared_ptr<AST> Parser::readSexpr() {
  const auto tokenType = lexer.nextToken();
  if (TokenType::START_PAREN != tokenType) {
    throw SyntaxError("Expected Sexpr start", lexer.line(), lexer.col());
  }
  return std::make_shared<ASTSexpr>();

  // switch (tokenType) {

  // case TokenType::SYMBOL:
  //   return std::make_shared<ASTSymbol>(lexer.string());
  // case TokenType::NONE:
  // case TokenType::TOKEN_EOF:
  // case TokenType::END_PAREN:
  // case TokenType::STRING:
  // case TokenType::INTEGER:
  // case TokenType::FLOAT:
  //   break;
  // }
  // assert(false);

  // return nullptr;
}
