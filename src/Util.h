#ifndef SHADER_UTIL_H_
#define SHADER_UTIL_H_

#include <ostream>
#include <string>
#include <vector>

#include "Lexer.h"

/// helping utilites
namespace util {
std::vector<Lexer::TokenType> tokenizeFile(std::string file);
}

namespace std {

ostream &operator<<(ostream &os, const Lexer::TokenType &t);

} // std

#endif /* !SHADER_UTIL_H_ */
