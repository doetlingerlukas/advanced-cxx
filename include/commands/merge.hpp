#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include <constants.hpp>
#include <command.hpp>
#include <diff.hpp>
#include <revision.hpp>
#include <commit.hpp>
#include <patch.hpp>

namespace fs = std::filesystem;

using namespace std;

class MergeCommand: public Command {
  public:
    string name() const override {
      return "merge";
    }

    string description() const override {
      return "Merges changes of two commits.";
    }

    int execute(vector<string> arguments) const override {
      if (arguments.size() != 2) {
        cerr << "Invalid amount of arguments supplied." << endl;
        return 1;
      }

      auto source_rev = Revision(arguments.front());
      auto target_rev = Revision(arguments.back());
      if (!(fs::exists(source_rev.directory()) && fs::exists(target_rev.directory()))) {
        cerr << "Unknown commit(s)." << endl;
        return 1;
      }

      Commit source = Commit::parse(source_rev.filepath().string());
      Commit target = Commit::parse(target_rev.filepath().string());
      auto source_parents = source.revision_history();
      auto target_parents = target.revision_history();
      fs::create_directory(fs::absolute(lit::MERGE_SOURCE));
      fs::create_directory(fs::absolute(lit::MERGE_TARGET));

      for (auto& r : source_parents) {
        Patch::apply(fs::absolute(r.patchpath()), fs::absolute(lit::MERGE_SOURCE));
      }
      for (auto& r : target_parents) {
        Patch::apply(fs::absolute(r.patchpath()), fs::absolute(lit::MERGE_TARGET));
      }

      // TODO: refactor
      Revision base_rev = target_parents.at(0);
      for (auto i = 0; i < target_parents.size(); i++) {
        try {
          if (source_parents.at(i) == target_parents.at(i)) {
            base_rev = target_parents.at(i);
          }
        } catch (std::out_of_range const& exc) {}
      }
      lit::Repository::checkout(base_rev);

      Diff source_diff(fs::absolute(lit::MERGE_SOURCE), true);
      source_diff.print_status();
      cout << "------------------------" << endl;
      Diff target_diff(fs::absolute(lit::MERGE_TARGET), true);
      target_diff.print_status();

      if (target_diff.conflicts_with(source_diff)) {
        cerr << "Merge conflict." << endl;
        return 1;
      }

      return 0;
    }
};