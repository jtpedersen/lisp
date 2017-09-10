#ifndef PARSER_H_
#define PARSER_H_
#include "AST.h"
#include "Lexer.h"

#include <exception>
#include <memory>

// "                                          \
  //   number : /-?[0-9]+/ ;                    \
  //   symbol : 'identifer'
//   sexpr  : '(' <expr>* ')' ;               \
  //   expr   : <number> | <symbol> | <sexpr> ; \
  //   lispy  : /^/ <expr>* /$/ ;               \
  // ",

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

  void syntaxError(const char *msg);
  std::shared_ptr<AST> readSexpr();
  std::shared_ptr<AST> readExpr();


  bool isBuiltin(const char *name) const;
  std::shared_ptr<ASTBuiltin> builtinFromName(const char *name);

};

#endif /* !PARSER_H_ */
