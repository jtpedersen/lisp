#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "StopWatch.h"
#include "Util.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;
const string PROMPT(">");
Interpreter interpreter;

void printEvaluation(const char *code) {
  Lexer l(code);
  Parser p(l);
  StopWatch parseTime;
  const auto program = p.read();
  parseTime.stop();
  for (const auto &e : program) {
    util::print(e);
  }
  cout << "\n(parse " << parseTime.elapsed() << "ms )" << std::endl;
  for (const auto &e : program) {
    util::print(e);
    cout << "\n";
    StopWatch evalTime;
    cout << interpreter.eval(e)->toString();
    evalTime.stop();
    cout << "\t\t\t(eval " << evalTime.elapsed() << "ms)" << endl;
  }
}

int runFile(const char *fileName) {
  std::ifstream ifs(fileName);
  // get length of file:
  ifs.seekg(0, ifs.end);
  int length = ifs.tellg();
  ifs.seekg(0, ifs.beg);

  char *buffer = new char[length];
  ifs.read(buffer, length);
  if (!ifs)
    std::cout << "error: only " << ifs.gcount() << " could be read";
  ifs.close();
  try {
    printEvaluation(buffer);
  } catch (const exception &e) {
    cout << "Error occurred: " << e.what() << endl;
    cout << "\n" << PROMPT;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc == 2) {
    return runFile(argv[1]);
  }

  string line;
  cout << "Velcome to shader lisp (built " << __DATE__ << ":" << __TIME__
       << ")\n"
       << PROMPT;

  while (getline(cin, line)) {
    try {
      printEvaluation(line.c_str());
      cout << "\n" << PROMPT;

    } catch (const exception &e) {
      cout << "Error occurred: " << e.what() << endl;
      cout << "\n" << PROMPT;
    }
  }
  return 0;
}
