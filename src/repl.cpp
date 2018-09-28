#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "StopWatch.h"
#include "Util.h"
#include "Environment.h"

#include "replxx.hxx"
#include <regex>
using Replxx = replxx::Replxx;

// the path to the history file
std::string history_file{"./replxx_history.txt"};


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
  cout << "\n(parse " << parseTime.elapsed() << " ms)" << std::endl;
  for (const auto &e : program) {
    util::print(e);
    cout << "\n";
    StopWatch evalTime;
    cout << interpreter.eval(e)->toString();
    evalTime.stop();
    cout << "\t\t\t(eval " << evalTime.elapsed() << " ms)" << endl;
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

// prototypes
Replxx::completions_t hook_completion(std::string const &context, int index,
                                      void *user_data);
Replxx::hints_t hook_hint(std::string const &context, int index,
                          Replxx::Color &color, void *user_data);
void hook_color(std::string const &str, Replxx::colors_t &colors,
                void *user_data);

Replxx::completions_t hook_completion(std::string const &context, int index,
                                      void *user_data) {
  const auto symbols = interpreter.environment()->symbols();
  Replxx::completions_t completions;
  std::string prefix{context.substr(index)};
  if (index > 1 && context[0] == '(')
    prefix = prefix.substr(1);
  for (const auto& e : symbols) {
    if (e.compare(0, prefix.size(), prefix) == 0) {
      completions.emplace_back(e.c_str());
    }
  }

  return completions;
}

Replxx::hints_t hook_hint(std::string const &context, int index,
                          Replxx::Color &color, void *user_data) {
  auto *examples = static_cast<std::vector<std::string> *>(user_data);
  Replxx::hints_t hints;

  // only show hint if prefix is at least 'n' chars long
  // or if prefix begins with a specific character
  std::string prefix{context.substr(index)};
  if (prefix.size() >= 2 || (!prefix.empty() && prefix.at(0) == '.')) {
    for (auto const &e : *examples) {
      if (e.compare(0, prefix.size(), prefix) == 0) {
        hints.emplace_back(e.substr(prefix.size()).c_str());
      }
    }
  }

  // set hint color to green if single match found
  if (hints.size() == 1) {
    color = Replxx::Color::GREEN;
  }

  return hints;
}

int utf8_strlen(std::string const &s) {
  int len(0);
  const auto m4 = static_cast<char>(128 + 64 + 32 + 16);
  const auto m3 = static_cast<char>(128 + 64 + 32);
  const auto m2 = static_cast<char>(128 + 64);
  for (int i(0); i < static_cast<int>(s.length()); ++i, ++len) {
    char c(s[i]);
    if ((c & m4) == m4) {
      i += 3;
    } else if ((c & m3) == m3) {
      i += 2;
    } else if ((c & m2) == m2) {
      i += 1;
    }
  }
  return (len);
}

void hook_color(std::string const &context, Replxx::colors_t &colors,
                void *user_data) {
  auto *regex_color =
      static_cast<std::vector<std::pair<std::string, Replxx::Color>> *>(
          user_data);

  // highlight matching regex sequences
  for (auto const &e : *regex_color) {
    size_t pos{0};
    std::string str = context;
    std::smatch match;

    while (std::regex_search(str, match, std::regex(e.first))) {
      std::string c{match[0]};
      pos += utf8_strlen(match.prefix());
      int len(utf8_strlen(c));

      for (int i = 0; i < len; ++i) {
        colors.at(pos + i) = e.second;
      }

      pos += len;
      str = match.suffix();
    }
  }
}

Replxx init_replxx() {

  // words to be completed
  std::vector<std::string> examples{
      ".help",
      ".history",
      ".quit",
      ".exit",
      ".clear",
      ".prompt ",
      "hello",
      "world",
      "db",
      "data",
      "drive",
      "print",
      "put",
      "color_black",
      "color_red",
      "color_green",
      "color_brown",
      "color_blue",
      "color_magenta",
      "color_cyan",
      "color_lightgray",
      "color_gray",
      "color_brightred",
      "color_brightgreen",
      "color_yellow",
      "color_brightblue",
      "color_brightmagenta",
      "color_brightcyan",
      "color_white",
      "color_normal",
  };

  // highlight specific words
  // a regex string, and a color
  // the order matters, the last match will take precedence
  using cl = Replxx::Color;
  std::vector<std::pair<std::string, cl>> regex_color{
      // single chars
      {"\\`", cl::BRIGHTCYAN},
      {"\\'", cl::BRIGHTBLUE},
      {"\\\"", cl::BRIGHTBLUE},
      {"\\-", cl::BRIGHTBLUE},
      {"\\+", cl::BRIGHTBLUE},
      {"\\=", cl::BRIGHTBLUE},
      {"\\/", cl::BRIGHTBLUE},
      {"\\*", cl::BRIGHTBLUE},
      {"\\^", cl::BRIGHTBLUE},
      {"\\.", cl::BRIGHTMAGENTA},
      {"\\(", cl::BRIGHTMAGENTA},
      {"\\)", cl::BRIGHTMAGENTA},
      {"\\[", cl::BRIGHTMAGENTA},
      {"\\]", cl::BRIGHTMAGENTA},
      {"\\{", cl::BRIGHTMAGENTA},
      {"\\}", cl::BRIGHTMAGENTA},

      // color keywords
      {"color_black", cl::BLACK},
      {"color_red", cl::RED},
      {"color_green", cl::GREEN},
      {"color_brown", cl::BROWN},
      {"color_blue", cl::BLUE},
      {"color_magenta", cl::MAGENTA},
      {"color_cyan", cl::CYAN},
      {"color_lightgray", cl::LIGHTGRAY},
      {"color_gray", cl::GRAY},
      {"color_brightred", cl::BRIGHTRED},
      {"color_brightgreen", cl::BRIGHTGREEN},
      {"color_yellow", cl::YELLOW},
      {"color_brightblue", cl::BRIGHTBLUE},
      {"color_brightmagenta", cl::BRIGHTMAGENTA},
      {"color_brightcyan", cl::BRIGHTCYAN},
      {"color_white", cl::WHITE},
      {"color_normal", cl::NORMAL},

      // commands
      {"\\.help", cl::BRIGHTMAGENTA},
      {"\\.history", cl::BRIGHTMAGENTA},
      {"\\.quit", cl::BRIGHTMAGENTA},
      {"\\.exit", cl::BRIGHTMAGENTA},
      {"\\.clear", cl::BRIGHTMAGENTA},
      {"\\.prompt", cl::BRIGHTMAGENTA},

      // numbers
      {"[\\-|+]{0,1}[0-9]+", cl::YELLOW},          // integers
      {"[\\-|+]{0,1}[0-9]*\\.[0-9]+", cl::YELLOW}, // decimals
      {"[\\-|+]{0,1}[0-9]+e[\\-|+]{0,1}[0-9]+",
       cl::YELLOW}, // scientific notation

      // strings
      {"\".*?\"", cl::BRIGHTGREEN}, // double quotes
      {"\'.*?\'", cl::BRIGHTGREEN}, // single quotes
  };

  // init the repl
  Replxx rx;
  rx.install_window_change_handler();


  //l oad the history file if it exists
  rx.history_load(history_file);

  // set the max history size
  rx.set_max_history_size(12);

  // set the max input line size
  rx.set_max_line_size(128);

  // set the max number of hint rows to show
  rx.set_max_hint_rows(8);

  // set the callbacks

  // rx.set_highlighter_callback(hook_color, static_cast<void *>(&regex_color));
  // 

  return rx;
}

int main(int argc, char *argv[]) {
  if (argc == 2) {
    return runFile(argv[1]);
  }

  auto rx = init_replxx();
  rx.set_completion_callback(hook_completion, nullptr);
  // rx.set_hint_callback(hook_completion, nullptr);
  
  // set the repl prompt
  std::string prompt{"\x1b[1;32mlispy\x1b[0m> "};

  // main repl loop
  for (;;) {
    // display the prompt and retrieve input from the user
    char const *cinput{nullptr};

    do {
      cinput = rx.input(prompt);
    } while ((cinput == nullptr) && (errno == EAGAIN));

    if (cinput == nullptr) {
      break;
    }

    // change cinput into a std::string
    // easier to manipulate
    std::string input{cinput};

    if (input.empty()) {
      // user hit enter on an empty line
      continue;
    }
    rx.history_add(cinput);
    try {
      printEvaluation(cinput);
    } catch (const exception &e) {
      cout << "Error occurred: " << e.what() << endl;
    }
  }

	// save the history
	rx.history_save(history_file);

  return 0;
}
