#ifndef SYNTAXERROR_H_
#define SYNTAXERROR_H_
#include "AST.h"
#include <exception>


class SyntaxError : public std::exception {
public:
  SyntaxError(const char *what, int line, int col);
  SyntaxError(const char *what, std::shared_ptr<AST> node);

  const char *what() const noexcept override;

private:
  const char *msg;
  int line;
  int col;
};

#endif /* !SYNTAXERROR_H_ */
