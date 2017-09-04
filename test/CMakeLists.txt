include_directories(../ src)

macro(TESTCASE casename)
add_executable(${casename} ${ARGN})
#Link test executable against gtest &gtest_main
target_link_libraries(${casename} gtest gmock)
add_test(${casename} ${casename})
endmacro(TESTCASE)

TESTCASE(frist frist.cpp)
