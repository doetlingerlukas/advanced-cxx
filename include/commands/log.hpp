#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <command.hpp>
#include <constants.hpp>
#include <log.hpp>

namespace fs = std::filesystem;

using namespace std;

class LogCommand : public Command {
  public:
  string name() const override {
    return "log";
  }

  string description() const override {
    return "Displays the current commit tree.";
  }

  int execute(vector<string> arguments) const override {
    if (arguments.size() > 0) {
      cerr << "No arguments expected for `lit log`." << endl;
      return 1;
    }

    lit::Log log;
    log.print();

    return 0;
  }
};