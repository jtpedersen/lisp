#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#include "AST.h"
#include <functional>

class Environment;

class Interpreter {
public:
  Interpreter();
  std::shared_ptr<AST> eval(std::shared_ptr<AST> node);

  const std::shared_ptr<Environment> environment() const;
private:
  std::shared_ptr<AST> evalBuiltin(std::shared_ptr<AST> node);

  /// The head of the child list has been evaled to determine operation
  /// @{
  std::shared_ptr<AST> evalIntOp(Builtin intOp, AST::List ls);
  std::shared_ptr<AST> evalDefine(const std::shared_ptr<AST> &node);

  std::shared_ptr<AST> evalList(const AST::List &ls);
  std::shared_ptr<AST> evalJoin(const std::shared_ptr<AST> &opnode,
                                const AST::List &ls);
  std::shared_ptr<AST> evalPPrint(const std::shared_ptr<AST> &opnode,
                                  const AST::List &ls);
  std::shared_ptr<AST> evalIf(const AST::List &ls);

  std::shared_ptr<AST> evalEq(AST::List ls);

  std::shared_ptr<AST>
  evalIntCmp(const AST::List &ls,
             std::function<bool(const int &, const int &)>);

  /// @}

  AST::List getEvaledArgs(const AST::List xs);

  void requireIntType(std::shared_ptr<AST> node);

  std::shared_ptr<AST> getSingleListArg(const std::shared_ptr<AST> &opnode,
                                        const AST::List &ls);

  void requireSingleArgument(const std::shared_ptr<AST> &opnode, AST::List ls);

  void requireListType(const std::shared_ptr<AST> &opnode,
                       std::shared_ptr<AST> node);
  void requireNonEmptyList(const std::shared_ptr<AST> &opnode,
                           std::shared_ptr<AST> node);

  int applyIntOp(const int acc, const AST::List ls, const size_t idx,
                 std::function<int(const int, const int)> op);

  /// Return name of symbol in node, throw error if not a symbol
  const char *symbolName(std::shared_ptr<AST> node);

  std::shared_ptr<Environment> env;
};

#endif /* !INTERPRETER_H_ */
