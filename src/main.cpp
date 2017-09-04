#include "Util.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cout << "Usage " << argv[0] << " file" << endl;
  }

  const auto tokens = util::tokenizeFile(argv[1]);
  for(const auto &token : tokens) {
    cout << token << endl;
  }

  return 0;
}
