
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -p -O0 -Wall -pedantic -Werror ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto -DNDEBUG")


add_library(
  lisp
  Lexer.cpp
  Parser.cpp
  Util.cpp
  Interpreter.cpp
  SyntaxError.cpp
  AST.cpp
  Environment.cpp
  StopWatch.cpp
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
