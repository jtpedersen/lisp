#include "Parser.h"
#include <cassert>

using TokenType = Lexer::TokenType;

Parser::Parser(const Lexer &lexer) : lexer(lexer) {}

std::shared_ptr<AST> Parser::read() {
  const auto tokenType = lexer.nextToken();
  switch (tokenType) {
  case TokenType::START_PAREN:
    return std::make_shared<ASTSexpr>();

  case TokenType::SYMBOL:
    return std::make_shared<ASTSymbol>(lexer.string());
  case TokenType::NONE:
  case TokenType::TOKEN_EOF:
  case TokenType::END_PAREN:
  case TokenType::STRING:
  case TokenType::INTEGER:
  case TokenType::FLOAT:
    break;
  }
  assert(false);

  return nullptr;
}
