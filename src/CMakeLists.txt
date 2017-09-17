add_library(
  lisp
  Lexer.cpp
  Parser.cpp
  Util.cpp
  Interpreter.cpp
  SyntaxError.cpp
  AST.cpp
  Environment.cpp
  )

add_executable(
  repl
  repl.cpp
  )

target_link_libraries(repl lisp)

add_executable(
  shader
  main.cpp
  )

target_link_libraries(shader lisp)
