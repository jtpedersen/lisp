#ifndef AST_H_
#define AST_H_
#include <memory>
#include <vector>

class AST {
public:
  using List = std::vector<std::shared_ptr<AST>>;
  enum class Type {
    SEXPR,
    SYMBOL,
    INTEGER,
    STRING,
    DEFINE,
    BUILTIN
  };

  explicit AST(Type type) : type_(type) {}

  Type type() const { return type_; };
  void setType(const Type &type) { type_ = type; };

  void setChildren(List children) { children_ = children; }
  void addChild(std::shared_ptr<AST> child) { children_.push_back(child); }
  List children() { return children_; }

  std::shared_ptr<AST> &operator[](std::size_t idx) { return children_[idx]; }
  const std::shared_ptr<AST> &operator[](std::size_t idx) const {
    return children_[idx];
  }

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
    case Type::BUILTIN:
      return "BUILTIN";
    case Type::DEFINE:
      return "DEFINE";
    }
  }

private:
  Type type_;
  List children_;
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
using ASTString = ASTValueNode<const char *, AST::Type::STRING>;
using ASTSymbol = ASTValueNode<const char *, AST::Type::SYMBOL>;
using ASTBuiltin = ASTValueNode<const char *, AST::Type::SYMBOL>;

#endif /* !AST_H_ */
