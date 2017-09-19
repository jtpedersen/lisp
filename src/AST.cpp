#include "AST.h"

const char *builtinToCString(Builtin op) {
  switch (op) {
  case Builtin::ADD:
    return "+";
  case Builtin::SUB:
    return "-";
  case Builtin::DIV:
    return "/";
  case Builtin::MUL:
    return "*";
  case Builtin::MOD:
    return "%";
  case Builtin::DEFINE:
    return "define";
  case Builtin::LIST:
    return "list";
  case Builtin::HEAD:
    return "head";
  case Builtin::TAIL:
    return "tail";
  case Builtin::JOIN:
    return "join";
  case Builtin::EVAL:
    return "eval";
  case Builtin::PPRINT:
    return "pprint";
  case Builtin::IF:
    return "if";
  case Builtin::EQ:
    return "=";
  case Builtin::GT:
    return ">";
  case Builtin::GE:
    return ">=";
  case Builtin::LT:
    return "<";
  case Builtin::LE:
    return "<=";

  case Builtin::UNKNOWN:
    return "UNKNOWN";
  }

  return "UNKNOWN";
}

Builtin builtinFromCString(const char *str) {
  if (0 == strcmp("+", str)) {
    return Builtin::ADD;
  } else if (0 == strcmp("-", str)) {
    return Builtin::SUB;
  } else if (0 == strcmp("/", str)) {
    return Builtin::DIV;
  } else if (0 == strcmp("*", str)) {
    return Builtin::MUL;
  } else if (0 == strcmp("%", str)) {
    return Builtin::MOD;
  } else if (0 == strcmp("define", str)) {
    return Builtin::DEFINE;
  } else if (0 == strcmp("list", str)) {
    return Builtin::LIST;
  } else if (0 == strcmp("head", str)) {
    return Builtin::HEAD;
  } else if (0 == strcmp("tail", str)) {
    return Builtin::TAIL;
  } else if (0 == strcmp("join", str)) {
    return Builtin::JOIN;
  } else if (0 == strcmp("eval", str)) {
    return Builtin::EVAL;
  } else if (0 == strcmp("pprint", str)) {
    return Builtin::PPRINT;
  } else if (0 == strcmp("if", str)) {
    return Builtin::IF;
  } else if (0 == strcmp("=", str)) {
    return Builtin::EQ;
  } else if (0 == strcmp(">", str)) {
    return Builtin::GT;
  } else if (0 == strcmp(">=", str)) {
    return Builtin::GE;
  } else if (0 == strcmp("<", str)) {
    return Builtin::LT;
  } else if (0 == strcmp("<=", str)) {
    return Builtin::LE;
  }

  return Builtin::UNKNOWN;
}
