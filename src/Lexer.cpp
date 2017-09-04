#include "Lexer.h"

Lexer::Lexer(cons char *const data, int dataSize)
    : data(data), dataSize(dataSize) {}

TokenType Lexer::nextToken() { return TokenType::NONE; }
