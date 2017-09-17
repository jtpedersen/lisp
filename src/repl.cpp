#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "Util.h"

#include <iostream>
#include <string>

using namespace std;
const string PROMPT(">");

int main(int argc, char *argv[]) {
  string line;
  cout << "Velcome to shader lisp (built " << __DATE__ << ":" << __TIME__
       << ")\n"
       << PROMPT;
  Interpreter interpreter;

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
