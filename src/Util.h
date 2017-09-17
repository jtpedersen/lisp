#ifndef SHADER_UTIL_H_
#define SHADER_UTIL_H_

#include <ostream>
#include <string>
#include <vector>

#include "AST.h"
#include "Lexer.h"

/// helping utilites
namespace util {
std::vector<Lexer::TokenType> tokenizeFile(std::string file);

void print(const std::shared_ptr<AST> &node);
void printList(const AST::List &ls);
}

namespace std {

ostream &operator<<(ostream &os, const Lexer::TokenType &t);
ostream &operator<<(ostream &os, const AST::Type &t);

} // std

#endif /* !SHADER_UTIL_H_ */
