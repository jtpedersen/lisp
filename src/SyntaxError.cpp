#include "SyntaxError.h"

SyntaxError::SyntaxError(const char *what, int line, int col)
    : msg(what), line(line), col(col) {}


SyntaxError::SyntaxError(const char *what, std::shared_ptr<AST> node)
  : msg(what) {} // TODO: from AST, line(line), col(col) {}


const char *SyntaxError::what() const noexcept {
  char *buf = static_cast<char *>(malloc(strlen(msg) + 1024));
  sprintf(buf, "%s on line %d:%d", msg, line, col);
  return buf;
}
