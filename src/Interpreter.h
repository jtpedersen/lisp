#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#include "AST.h"


std::shared_ptr<AST> eval(std::shared_ptr<AST> node);

#endif /* !INTERPRETER_H_ */
