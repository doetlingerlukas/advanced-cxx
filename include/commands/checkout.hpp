#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include <constants.hpp>
#include <command.hpp>

namespace fs = std::filesystem;

using namespace std;

class CheckoutCommand: public Command {
  public:
    string name() const {
      return "checkout";
    }

    string description() const {
      return "Resets the state of all files to the given commit's state. All un-committed changes are dropped upon checkout.";
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

      

      return 0;
    }
};