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

      Commit commit(revision, nullopt, nullopt, arguments.at(0));
      commit.save();

      fs::remove_all(fs::current_path() / lit::PREVIOUS_DIR);
      fs::create_directories(lit::PREVIOUS_DIR);
      for (auto& p : fs::directory_iterator(fs::current_path())) {
        if (p.path().filename().string() != ".lit") {
          const auto copy_options = fs::copy_options::overwrite_existing
            | fs::copy_options::recursive;
          fs::copy(p, fs::absolute(lit::PREVIOUS_DIR) / fs::relative(p.path(), fs::current_path()), copy_options);
        }
      }

      return 0;
    }
};