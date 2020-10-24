#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

#include <constants.hpp>
#include <command.hpp>

namespace fs = std::filesystem;

using namespace std;

class Show: public Command {
  public:
    string name() const {
      return "show";
    }

    string description() const {
      return "Inspect a given commit.";
    }

    int execute(vector<string> arguments) const {
      if (arguments.size() != 1) {
        cerr << "Invalid amount of arguments supplied." << endl;
        return 1;
      }

      fs::path commit_dir = string(lit::REVISION_DIR) + "/" + arguments.front();
      if (!fs::exists(commit_dir)) {
        cerr << "Unknown commit." << endl;
        return 1;
      }

      {
        ifstream commit_file(commit_dir.string() + "/" + arguments.front() + ".lit");
        if (commit_file.is_open()) {
          cout << commit_file.rdbuf();
        }
      }

      cout << endl;

      {
        ifstream patch_file(commit_dir.string() + "/.patch");
        if (patch_file.is_open()) {
          cout << patch_file.rdbuf();
        }
      }

      return 0;
    }
};