#ifndef LEXER_H_
#define LEXER_H_
#include <vector>

/// Must work in arduino so no std::strings
class Lexer {
public:
  enum class TokenType {
    NONE,
    TOKEN_EOF,
    START_PAREN,
    END_PAREN,
    SYMBOL,
    STRING,
    INTEGER,
    FLOAT
  };
  Lexer(const char *const data);
  TokenType nextToken();

  /// The value of a string
  const char *const string();

private:
  const char *data;
  const char *pos;
  char current;
  bool good;

  TokenType readString();
  TokenType readAtom(char c);

  std::vector<char> buffer;
  char nextChar();
};

#endif /* !LEXER_H_ */
