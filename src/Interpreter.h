#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#include "AST.h"

class Environment;

class Interpreter {
public:
  Interpreter();
  std::shared_ptr<AST> eval(std::shared_ptr<AST> node);

private:
  std::shared_ptr<AST> evalIntOp(Builtin intOp, AST::List ls);
  std::shared_ptr<AST> evalBuiltin(AST::List ls);
  void requireIntType(std::shared_ptr<AST> node);
  bool isIntOp(Builtin op);

  std::shared_ptr<AST> getSingleListArg(const std::shared_ptr<AST> &opnode,
                                             const AST::List &ls);

  void requireSingleArgument(const std::shared_ptr<AST> &opnode,
                                  AST::List ls);

  void requireListType(const std::shared_ptr<AST> &opnode,
                            std::shared_ptr<AST> node);
  void requireNonEmptyList(const std::shared_ptr<AST> &opnode,
                                std::shared_ptr<AST> node);

  int applyIntOp(const int acc, const AST::List ls, const size_t idx,
                      std::function<int(const int, const int)> op);

  std::shared_ptr<Environment> env;


};

#endif /* !INTERPRETER_H_ */
