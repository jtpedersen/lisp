#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "AST.h"

#include <cstring>
#include <map>
#include <memory>

class Environment {
public:
  explicit Environment(std::shared_ptr<Environment> parent = nullptr);
  ~Environment();
  void setEntry(const char *symbol, std::shared_ptr<AST> node);
  std::shared_ptr<AST> operator[](const char *symbol);

private:
  struct cmp_str {
    bool operator()(char const *a, char const *b) {
      return std::strcmp(a, b) < 0;
    }
  };
  std::map<const char *, std::shared_ptr<AST>, cmp_str> env;
  std::shared_ptr<Environment> parent;
};

#endif /* !ENVIRONMENT_H_ */
