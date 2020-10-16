#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "command.hpp"

namespace fs = std::filesystem;

using namespace std;

class Init: public Command {
  public:
    string name() const {
      return "init";
    }

    string description() const {
      return "Initializes a repository.";
    }

    int execute(vector<string> arguments) const {
      if (fs::exists(".lit")) {
        cerr << "Repository already initialized at " << fs::absolute(".lit") << endl;
        return 1;
      }

      fs::create_directory(".lit");
      cout << "Initialized lit repository at " << fs::absolute(".lit") << endl;

      return 0;
    }
};