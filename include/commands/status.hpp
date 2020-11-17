#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <command.hpp>
#include <diff.hpp>

namespace lit {

using namespace std;

class StatusCommand : public Command {
  public:
  string name() const override {
    return "status";
  }

  string description() const override {
    return "Lists all files that have been added, removed, or modified, with respect to the currently checked out "
           "commit.";
  }

  int execute(vector<string> arguments) const override {
    if (arguments.size() > 0) {
      cerr << "No arguments expected!" << endl;
      return 1;
    }

    Diff diff;
    diff.print_status();
    return 0;
  }
};

} // namespace lit