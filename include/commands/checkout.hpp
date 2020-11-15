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

using namespace std;

class CheckoutCommand : public Command {
  public:
  string name() const {
    return "checkout";
  }

  string description() const {
    return "Resets the state of all files to the given commit's state. All un-committed changes are dropped upon "
           "checkout.";
  }

  int execute(vector<string> arguments) const {
    if (arguments.size() > 1) {
      cerr << "Invalid amount of arguments supplied." << endl;
      return 1;
    }

    if (arguments.size() == 0 && !lit::Repository::get_head().has_value()) {
      lit::Repository::clear();
      return 0;
    }

    const auto revision =
        arguments.size() == 1 ? Revision(arguments.front()) : Revision(lit::Repository::current_index().value());
    if (!fs::exists(revision.directory())) {
      cerr << "Unknown commit." << endl;
      return 1;
    }

    lit::Repository::checkout(revision);

    cout << "Checkout successful!" << endl;
    cout << "HEAD is now at " << revision.to_string() << endl;

    return 0;
  }
};