#include "Lexer.h"
#include <cassert>
#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Lexer::Lexer(const char *const data)
    : data(data), pos(data), good(true), current('\0'), line_(0), col_(0) {}

Lexer::TokenType Lexer::nextToken() {
  nextChar();
  if (!good)
    return TokenType::TOKEN_EOF;
  // if (';' == current) {
  //   while (current != '\n') {
  //     nextChar();
  //     if (!good)
  //       return TokenType::TOKEN_EOF;
  //   }
  // }
  if (isspace(current))
    return nextToken();

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
  if ('\n' == current) {
    line_++;
    col_ = 1;
  } else {
    col_++;
  }
  return good;
}

char Lexer::peek() const { return *(pos); }

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
}

Lexer::TokenType Lexer::readAtom() {
  buffer.clear();
  do {
    if (isspace(current)) {
      break;
    }
    buffer.emplace_back(current);
    if (isparen(peek()))
      break;
  } while (nextChar());

  if (isdigit(buffer.front()))
    return TokenType::INTEGER;
  if ('-' == buffer.front() && buffer.size() > 1)
    return TokenType::INTEGER;
  return TokenType::SYMBOL;
}

bool Lexer::isparen(char c) const { return ('(' == c) || (')' == c); }

const char *Lexer::string() {
  buffer.emplace_back('\0');
  return buffer.data();
}

int Lexer::integer() { return atoi(string()); }

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

int Lexer::line() const {
  // zero indexed
  return line_ + 1;
}

int Lexer::col() const {
  // the pointer is after what was read
  return col_ - 1;
}
