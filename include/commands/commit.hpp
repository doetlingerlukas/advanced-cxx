#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <command.hpp>
#include <commit.hpp>
#include <constants.hpp>
#include <diff.hpp>
#include <repository.hpp>
#include <revision.hpp>

namespace fs = std::filesystem;

namespace lit {

using namespace std;

class CommitCommand : public Command {
  public:
  string name() const override {
    return "commit";
  }

  string description() const override {
    return "Create a Commit for all current changes.";
  }

  int execute(vector<string> arguments) const override {
    if (false) {
      cerr << "Wrong number of arguments!" << endl;
      return 1;
    }

    Revision revision(lit::Repository::update_index());
    fs::create_directories(revision.directory());

    Diff diff;
    diff.save(revision.patchpath().string());

    Commit commit(revision, lit::Repository::get_head(), lit::Repository::get_parent_from_merge_conflict(),
                  arguments.at(0));
    commit.save();

    lit::Repository::set_head(revision);
    lit::Repository::set_previous_dir();

    cout << "Commit: " << revision.to_string() << endl;
    auto time = chrono::system_clock::to_time_t(commit.timestamp());
    cout << "Date: " << ctime(&time) << endl;

    return 0;
  }
};

} // namespace lit