include_directories(../src)

macro(TESTCASE casename)
add_executable(${casename} ${ARGN})
#Link test executable against gtest &gtest_main
target_link_libraries(${casename} gtest gmock)
add_test(${casename} ${casename})
endmacro(TESTCASE)

TESTCASE(frist frist.cpp)

TESTCASE(lexer Lexer.cpp ../src/Lexer.cpp)

TESTCASE(parser Parser.cpp ../src/Lexer.cpp ../src/Parser.cpp ../src/Util.cpp)
