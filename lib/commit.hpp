#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>

#include <command.hpp>
#include <diff.hpp>

namespace fs = std::filesystem;

using namespace std;

class Commit: public Command {
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

      auto revision = 0;
      for (auto& d : fs::directory_iterator(".lit/revisions")) {
        revision++;
      }

      string revision_dir(".lit/revisions/r" + to_string(revision));
      fs::create_directories(revision_dir);

      Diff diff;
      diff.save(revision_dir + "/.patch");
      
      auto time = chrono::system_clock::to_time_t(chrono::system_clock::now());
      ofstream commit_file;
      commit_file.open(revision_dir + "/r" + to_string(revision) + ".lit");
      commit_file << "message='" << arguments.at(0) << "'" << endl;
      commit_file << "time=" << put_time(localtime(&time), "%Y-%m-%d %X") << endl;
      commit_file << "parent=" << (revision > 0 ? ("r" + to_string(revision)) : "none") << endl;
      commit_file.close();

      fs::remove_all(fs::current_path() / ".lit/previous");
      fs::create_directories(".lit/previous");
      for (auto& p : fs::directory_iterator(fs::current_path())) {
        if (p.path().filename().string() != ".lit") {
          const auto copy_options = fs::copy_options::overwrite_existing
            | fs::copy_options::recursive;
          fs::copy(p, fs::absolute(".lit/previous") / fs::relative(p.path(), fs::current_path()), copy_options);
        }
      }

      return 0;
    }
};