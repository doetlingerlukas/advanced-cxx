#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include <constants.hpp>
#include <command.hpp>
#include <diff.hpp>
#include <revision.hpp>

namespace fs = std::filesystem;

using namespace std;

class MergeCommand: public Command {
  public:
    string name() const override {
      return "merge";
    }

    string description() const override {
      return "Merges changes of two commits.";
    }

    int execute(vector<string> arguments) const override {
      if (arguments.size() != 2) {
        cerr << "Invalid amount of arguments supplied." << endl;
        return 1;
      }

      auto from_rev = Revision(arguments.front());
      auto to_rev = Revision(arguments.back());
      if (!(fs::exists(from_rev.directory()) && fs::exists(to_rev.directory()))) {
        cerr << "Unknown commit(s)." << endl;
        return 1;
      }

      


      return 0;
    }
};