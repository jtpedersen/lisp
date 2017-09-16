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
  }
  return Builtin::UNKNOWN;
}
