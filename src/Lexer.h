#ifndef LEXER_H_
#define LEXER_H_

/// Must work in arduino so no std::strings
class Lexer {
  enum class TokenType { NONE, START_PAREN, END_PAREN, STRING, INTEGER, FLOAT };
  Lexer(const char *const data, int dataSize);
  TokenType nextToken();

private:
  const char const *data;
  int dataSize;
};

#endif /* !LEXER_H_ */
