#include "Lexer.h"
#include <cassert>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Lexer::Lexer(const char *const data)
    : data(data), pos(data), good(true), current('\0') {}

Lexer::TokenType Lexer::nextToken() {
  nextChar();
  if (!good)
    return TokenType::TOKEN_EOF;
  // printf("c %c\n", c);
  switch (current) {
  case '\0':
    return TokenType::TOKEN_EOF;
  case '(':
    return TokenType::START_PAREN;
  case ')':
    return TokenType::END_PAREN;
  case '"':
    return readString();
  default:
    return readAtom();
  }

  return TokenType::NONE;
}

bool Lexer::nextChar() {
  current = *pos++;
  if ('\0' == current) {
    good = false;
  }
  return good;
}

Lexer::TokenType Lexer::readString() {
  buffer.clear();
  bool escape = false;
  while (nextChar()) {
    if ('\\' == current) {
      escape = true;
      continue;
    }

    if (!escape && '"' == current) {
      break;
    }

    escape = false;
    buffer.emplace_back(current);
  }
  return good ? TokenType::STRING : TokenType::TOKEN_EOF;
  ;
}

Lexer::TokenType Lexer::readAtom() {
  buffer.clear();
  const auto ret = (isdigit(current) || '-' == current) ? TokenType::INTEGER : TokenType::SYMBOL;
  do {
    if (!isalnum(current) && '-' != current) {
      break;
    }
    buffer.emplace_back(current);
  } while (nextChar());
  return ret;
}

const char *Lexer::string() {
  buffer.emplace_back('\0');
  return buffer.data();
}

int Lexer::integer() {
  return atoi(string());
}

const char *Lexer::TokenToCString(const TokenType &tokenType) {
  switch (tokenType) {
  case TokenType::NONE:
    return "NONE";
  case TokenType::TOKEN_EOF:
    return "TOKEN_EOF";
  case TokenType::START_PAREN:
    return "START_PAREN";
  case TokenType::END_PAREN:
    return "END_PAREN";
  case TokenType::SYMBOL:
    return "SYMBOL";
  case TokenType::STRING:
    return "STRING";
  case TokenType::INTEGER:
    return "INTEGER";
  case TokenType::FLOAT:
    return "FLOAT";
  }
  assert(false);
  return "OH NOES";
}
