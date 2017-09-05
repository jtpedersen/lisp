#ifndef AST_H_
#define AST_H_
#include <memory>
#include <vector>

class AST {
public:
  enum class Type {
    SEXPR,
    SYMBOL,
    INTEGER,
    STRING,
    FUN,
  };

  explicit AST(Type type) : type_(type) {}

  Type type() const { return type_; };

  void setChildren(std::vector<std::shared_ptr<AST>> children) {
    children_ = children;
  }

  std::vector<std::shared_ptr<AST>> children() { return children_; }

  static const char *TypeToCString(Type t) {
    switch (t) {
    case Type::SEXPR:
      return "SEXPR";
    case Type::SYMBOL:
      return "SYMBOL";
    case Type::INTEGER:
      return "INTEGER";
    case Type::STRING:
      return "STRING";
    case Type::FUN:
      return "FUN";
    }
  }

private:
  Type type_;
  std::vector<std::shared_ptr<AST>> children_;
};

class ASTSexpr : public AST {
public:
  explicit ASTSexpr() : AST(Type::SEXPR) {}
};

template <typename ValueType, AST::Type TypeType>
class ASTValueNode : public AST {
public:
  explicit ASTValueNode(const ValueType &value)
      : AST(TypeType), value_(value) {}

  const ValueType &value() const { return value_; };

private:
  ValueType value_;
};

using ASTInt = ASTValueNode<int, AST::Type::INTEGER>;
using ASTSTring = ASTValueNode<const char *, AST::Type::STRING>;
using ASTSymbol = ASTValueNode<const char *, AST::Type::SYMBOL>;

#endif /* !AST_H_ */
