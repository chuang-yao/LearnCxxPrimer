//
//  Chapter 08 - I/O
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct PersonInfo {
  std::string name;
  std::vector<std::string> phones;
};

int main() {
  { std::cout << "Hello World!" << std::endl; }

  {
    std::cout << "hi!"
              << std::endl; // writes hi and a newline, then flushes the buffer
    std::cout << "hi!"
              << std::flush; // writes hi, then flushes the buffer; adds no data
    std::cout << "hi!"
              << std::ends; // writes hi and a null, then flushes the buffer
  }

  {
    std::cout << std::unitbuf;   // all writes will be flushed immediately
    std::cout << std::nounitbuf; // returns to normal buffering
  }

  {
    std::string line, word;
    std::vector<PersonInfo> people;

    while (getline(std::cin, line)) {
      PersonInfo info;
      std::istringstream record(line);
      record >> info.name;
      while (record >> word) {
        info.phones.push_back(word);
      }
      people.push_back(info);
    }
  }
}