#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <command.hpp>
#include <commit.hpp>
#include <constants.hpp>
#include <revision.hpp>

namespace fs = std::filesystem;

namespace lit {

using namespace std;

class ShowCommand : public Command {
  public:
  string name() const override {
    return "show";
  }

  string description() const override {
    return "Inspect a given commit.";
  }

  int execute(vector<string> arguments) const override {
    if (arguments.size() != 1) {
      cerr << "Invalid amount of arguments supplied." << endl;
      return 1;
    }

    Revision revision(arguments.front());
    if (!fs::exists(revision.directory())) {
      cerr << "Unknown commit." << endl;
      return 1;
    }

    auto commit = Commit::parse(revision.filepath().string());
    commit.print();

    cout << endl;

    {
      ifstream patch_file(revision.patchpath());
      if (patch_file.is_open()) {
        cout << patch_file.rdbuf();
      }
    }

    return 0;
  }
};

} // namespace lit