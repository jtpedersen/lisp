include_directories(../src)

macro(TESTCASE casename)
add_executable(${casename} ${ARGN})
#Link test executable against gtest &gtest_main
target_link_libraries(${casename} gtest gmock)
add_test(${casename} ${casename})
endmacro(TESTCASE)

TESTCASE(frist frist.cpp)

TESTCASE(lexer Lexer.cpp ../src/Lexer.cpp)
target_link_libraries(lexer lisp)

TESTCASE(parser Parser.cpp)
target_link_libraries(parser lisp)

TESTCASE(interpreter Interpreter.cpp)
target_link_libraries(interpreter lisp)
