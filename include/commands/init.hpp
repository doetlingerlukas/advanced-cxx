#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <command.hpp>
#include <constants.hpp>

namespace fs = std::filesystem;

using namespace std;

class InitCommand : public Command {
  public:
  string name() const {
    return "init";
  }

  string description() const {
    return "Initializes a repository.";
  }

  int execute(vector<string> arguments) const {
    if (fs::exists(lit::DIR)) {
      cerr << "Repository already initialized at " << fs::absolute(lit::DIR) << endl;
      return 1;
    }

    fs::create_directories(lit::REVISION_DIR);
    fs::create_directories(lit::PREVIOUS_DIR);
    cout << "Initialized lit repository at " << fs::absolute(lit::DIR) << endl;

    return 0;
  }
};