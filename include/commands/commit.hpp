#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>

#include <constants.hpp>
#include <command.hpp>
#include <diff.hpp>
#include <commit.hpp>
#include <revision.hpp>
#include <repository.hpp>

namespace fs = std::filesystem;

using namespace std;

class CommitCommand: public Command {
  public:
    string name() const {
      return "commit";
    }

    string description() const {
      return "Create a Commit for all current changes.";
    }

    int execute(vector<string> arguments) const {
      if (false) {
        cerr << "Wrong number of arguments!" << endl;
        return 1;
      }

      auto revision_count = 0;
      for (auto& d : fs::directory_iterator(lit::REVISION_DIR)) {
        revision_count++;
      }

      Revision revision(revision_count);
      fs::create_directories(revision.directory());

      Diff diff;
      diff.save(revision.patchpath().string());

      Commit commit(revision, lit::Repository::get_head(), nullopt, arguments.at(0));
      commit.save();

      lit::Repository::set_head(revision);
      lit::Repository::clear_previous_state();
      lit::Repository::set_previous_state();

      return 0;
    }
};