#ifndef PARSER_H_
#define PARSER_H_
#include "AST.h"
#include "Lexer.h"

#include <memory>
#include <exception>

class SyntaxError : public std::exception {
public:
  SyntaxError(const char *what, int line, int col);

  const char *what() const noexcept override;
private:
  const char *msg;
  int line;
  int col;
};

class Parser {
public:
  explicit Parser(const Lexer &lexer);

  std::shared_ptr<AST> read();

private:
  Lexer lexer;

  std::shared_ptr<AST> readSexpr();
};

#endif /* !PARSER_H_ */
