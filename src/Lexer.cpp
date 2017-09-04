#include "Lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

Lexer::Lexer(const char *const data, int dataSize)
    : data(data), dataSize(dataSize), pos(data), good(true) {}

Lexer::TokenType Lexer::nextToken() {
  auto c = nextChar();
  if (!good)
    return TokenType::TOKEN_EOF;
  // printf("c %c\n", c);
  switch (c) {
  case '\0':
    return TokenType::TOKEN_EOF;
  case '(':
    return TokenType::START_PAREN;
  case ')':
    return TokenType::END_PAREN;
  case '"': {
    buffer.clear();
    bool escape = false;
    while (c = nextChar()) {
      if (!good)
        return TokenType::TOKEN_EOF;
      //      printf("c %c\n", c);
      if ('\\' == c) {
        escape = true;
        continue;
      }

      if (!escape && '"' == c) {
        break;
      }

      escape = false;
      buffer.emplace_back(c);
    }
    return TokenType::STRING;
  }
  default:
    buffer.clear();
    do {
      if (!good)
        return TokenType::TOKEN_EOF;
      //      printf("c %c\n", c);
      if (!isalnum(c)) {
        break;
      }
      buffer.emplace_back(c);
    } while (c = nextChar());
    return TokenType::SYMBOL;
  }

  return TokenType::NONE;
}

char Lexer::nextChar() {
  const auto ret = *pos++;
  if ('\0' == ret) {
    good = false;
  }
  return ret;
}

const char *const Lexer::string() {
  buffer.emplace_back('\0');
  return buffer.data();
}
