#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "Util.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;
const string PROMPT(">");
Interpreter interpreter;

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
    Lexer l(buffer);
    Parser p(l);
    const auto program = p.read();
    util::print(program);
    cout << "\n";
    cout << interpreter.eval(program)->toString() << endl;
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
      Lexer l(line.c_str());
      Parser p(l);
      const auto program = p.read();
      util::print(program);
      cout << "\n";
      cout << "\t" << interpreter.eval(program)->toString() << endl;
      cout << "\n" << PROMPT;
    } catch (const exception &e) {
      cout << "Error occurred: " << e.what() << endl;
      cout << "\n" << PROMPT;
    }
  }
  return 0;
}
