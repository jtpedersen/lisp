#ifndef PARSER_H_
#define PARSER_H_
#include "Lexer.h"
#include "AST.h"

#include <memory>

class Parser {
public:
  explicit Parser(const Lexer& lexer);

  std::shared_ptr<AST> read();

private:
  Lexer lexer;
};

#endif /* !PARSER_H_ */
