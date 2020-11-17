#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <command.hpp>
#include <constants.hpp>

namespace fs = std::filesystem;

namespace lit {

using namespace std;

class InitCommand : public Command {
  public:
  string name() const override {
    return "init";
  }

  string description() const override {
    return "Initializes a repository.";
  }

  int execute(vector<string> arguments) const override {
    if (fs::exists(DIR)) {
      cerr << "Repository already initialized at " << fs::absolute(DIR) << endl;
      return 1;
    }

    fs::create_directories(REVISION_DIR);
    fs::create_directories(PREVIOUS_DIR);
    cout << "Initialized lit repository at " << fs::absolute(DIR) << endl;

    return 0;
  }
};

} // namespace lit