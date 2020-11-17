#include <deque>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <command.hpp>
#include <commit.hpp>
#include <constants.hpp>
#include <patch.hpp>
#include <repository.hpp>
#include <revision.hpp>

namespace fs = std::filesystem;

namespace lit {

using namespace std;

class CheckoutCommand : public Command {
  public:
  string name() const override {
    return "checkout";
  }

  string description() const override {
    return "Resets the state of all files to the given commit's state. All un-committed changes are dropped upon "
           "checkout.";
  }

  int execute(vector<string> arguments) const override {
    if (arguments.size() > 1) {
      cerr << "Invalid amount of arguments supplied." << endl;
      return 1;
    }

    if (arguments.size() == 0 && !Repository::get_head().has_value()) {
      Repository::clear();
      return 0;
    }

    const auto revision =
        arguments.size() == 1 ? Revision(arguments.front()) : Revision(Repository::current_index().value());
    if (!fs::exists(revision.directory())) {
      cerr << "Unknown commit." << endl;
      return 1;
    }

    Repository::checkout(revision);

    cout << "Checkout successful!" << endl;
    cout << "HEAD is now at " << revision.to_string() << endl;

    return 0;
  }
};

} // namespace lit