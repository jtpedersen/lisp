#include "Lexer.h"
#include "Parser.h"
#include <iostream>
#include <string>

using namespace std;
const string PROMPT(">");

int main(int argc, char *argv[]) {
  string line;
  cout << "Velcome to shader lisp" << __DATE__ << ":" << __TIME__ << "\n"
       << PROMPT;
  while (getline(cin, line)) {
    try {
      Lexer l(line.c_str());
      Parser p(l);
      const auto  program = p.read();
      //      cout << eval(program)) << endl;
      cout << "\n" << PROMPT;
    } catch (const exception &e) {
      cout << "Error occurred: " << e.what();
    }
  }
  return 0;
}
