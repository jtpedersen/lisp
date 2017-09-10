#ifndef AST_H_
#define AST_H_
#include <memory>
#include <string.h>
#include <vector>

class AST {
public:
  using List = std::vector<std::shared_ptr<AST>>;
  enum class Type { SEXPR, SYMBOL, INTEGER, STRING, DEFINE, BUILTIN };

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

template <typename DataType, AST::Type TypeType>
class ASTDataNode : public AST {
public:
  explicit ASTDataNode(const DataType &data)
      : AST(TypeType), data_(data) {}

  const DataType &data() const { return data_; };

private:
  DataType data_;
};

using ASTInt = ASTDataNode<int, AST::Type::INTEGER>;
using ASTString = ASTDataNode<const char *, AST::Type::STRING>;
using ASTSymbol = ASTDataNode<const char *, AST::Type::SYMBOL>;

class ASTBuiltin : public AST {
public:
  enum class Operator {
    ADD,
    SUB,
    DIV,
    MUL,
    MOD,
    UNKNOWN,
    // TODO && || etc
  };
  explicit ASTBuiltin(Operator op) : AST(Type::BUILTIN), op_(op) {}
  Operator op() const { return op_; }

  static const char *operatorToCString(Operator op) {
    switch (op) {
    case Operator::ADD:
      return "ADD";
    case Operator::SUB:
      return "SUB";
    case Operator::DIV:
      return "DIV";
    case Operator::MUL:
      return "MUL";
    case Operator::MOD:
      return "MOD";
    case Operator::UNKNOWN:
      return "UNKNOWN";
    }
  }

  static Operator operatorFromCString(const char *str) {
    if (0 == strcmp("+", str)) {
      return Operator::ADD;
    } else if (0 == strcmp("-", str)) {
      return Operator::SUB;
    } else if (0 == strcmp("/", str)) {
      return Operator::DIV;
    } else if (0 == strcmp("*", str)) {
      return Operator::MUL;
    }
    return Operator::UNKNOWN;
  }

private:
  Operator op_;
};

#endif /* !AST_H_ */
