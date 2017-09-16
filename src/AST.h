#ifndef AST_H_
#define AST_H_
#include <memory>
#include <string.h>
#include <vector>

enum class Builtin {
  ADD,
  SUB,
  DIV,
  MUL,
  MOD,
  DEFINE,
  LIST,
  UNKNOWN,
  // TODO && || etc
};

const char *builtinToCString(Builtin op);
Builtin builtinFromCString(const char *str);

class AST {
public:
  using List = std::vector<std::shared_ptr<AST>>;
  enum class Type { SEXPR, SYMBOL, INTEGER, STRING, BUILTIN };

  explicit AST(Type type) : type_(type) {}

  Type type() const { return type_; };
  void setType(const Type &type) { type_ = type; };

  void setChildren(List children) { children_ = children; }
  void addChild(std::shared_ptr<AST> child) { children_.push_back(child); }
  List children() { return children_; }
  std::shared_ptr<AST> head() const { return children_.front(); }

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
    }
  }

  virtual const char *toString() { return TypeToCString(type()); }

  virtual bool isBuiltin(Builtin /* not used */ ) const { return false; } 

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
  explicit ASTDataNode(const DataType &data) : AST(TypeType), data_(data) {}
  const DataType &data() const { return data_; };

private:
  DataType data_;
};

class ASTInt : public ASTDataNode<int, AST::Type::INTEGER> {
public:
  explicit ASTInt(int data) : ASTDataNode(data){};
  const char *toString() override {
    char buf[1024];
    snprintf(buf, 1024, "%d", data());
    return strdup(buf);
  }
};

class ASTSymbol : public ASTDataNode<const char *, AST::Type::SYMBOL> {
public:
  explicit ASTSymbol(const char *data) : ASTDataNode(data){};
  const char *toString() override { return data(); }
};

class ASTString : public ASTDataNode<const char *, AST::Type::STRING> {
public:
  explicit ASTString(const char *data) : ASTDataNode(data){};
  const char *toString() override {
    char buf[1024];
    snprintf(buf, 1024, "\"%s\"", data());
    return strdup(buf);
  }
};

class ASTBuiltin : public AST {
public:
  explicit ASTBuiltin(Builtin op) : AST(Type::BUILTIN), op_(op) {}
  Builtin op() const { return op_; }
  const char *toString() override { return builtinToCString(op()); }
  virtual bool isBuiltin(Builtin op) const override { return op == op_; }

private:
  Builtin op_;
};

#endif /* !AST_H_ */
