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
  explicit Lexer(const char *const data);
  TokenType nextToken();

  /// The value of a string
  const char *string();
  /// The Value as Integer
  int integer();

  /// Current line in src
  int line() const;
  int col() const;

  static const char *TokenToCString(const TokenType &tokenType);

private:
  const char *data;
  const char *pos;
  char current;
  bool good;
  int line_;
  int col_;

  TokenType readString();
  TokenType readAtom();

  std::vector<char> buffer;
  bool nextChar();
  char peek() const;
  bool isparen(char c) const;
};

#endif /* !LEXER_H_ */
