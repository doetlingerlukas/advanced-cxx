#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <deque>

#include <constants.hpp>
#include <command.hpp>
#include <revision.hpp>
#include <commit.hpp>
#include <repository.hpp>
#include <patch.hpp>

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

      const Revision revision(arguments.front());
      if (!fs::exists(revision.directory())) {
        cerr << "Unknown commit." << endl;
        return 1;
      }

      Commit commit = Commit::parse(revision.filepath().string());
      deque<Revision> parents;

      while (commit.parent().has_value()) {
        parents.push_front(commit.parent().value());
        commit = Commit::parse(commit.parent().value().filepath().string());
      }

      lit::Repository::discard_changes();
      lit::Repository::clear_previous_state();
      for (auto& r : parents) {
        Patch::apply(r.patchpath());
      }
      lit::Repository::set_previous_state();
      lit::Repository::set_head(revision);

      cout << "Checkout successful!" << endl;
      cout << "HEAD is now at " << revision.to_string() << endl;

      return 0;
    }
};