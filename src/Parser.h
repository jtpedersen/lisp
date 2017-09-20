#ifndef PARSER_H_
#define PARSER_H_
#include "AST.h"
#include "Lexer.h"

#include <memory>

class Parser {
public:
  explicit Parser(const Lexer &lexer);

  AST::List read();

private:
  Lexer lexer;
  int depth;
  void checkSyntaxForBuiltin(AST::List ls) const;
  void syntaxError(const char *msg);
  std::shared_ptr<AST> readSexpr();
  std::shared_ptr<AST> readExpr();
};

#endif /* !PARSER_H_ */
