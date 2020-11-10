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
      if (arguments.size() != 1) {
        cerr << "Invalid amount of arguments supplied." << endl;
        return 1;
      }

      const auto source = Revision(arguments.front());
      const auto head = lit::Repository::get_head().value_or(Revision(0));
      if (!(source.exists() && head.exists()) || source == head) {
        cerr << "Incompatiple commit supplied." << endl;
        cerr << source.to_string() << " cannot be merged into HEAD " << head.to_string() << endl;;
        return 1;
      }

      const auto source_commit = Commit::parse(source.filepath().string());
      const auto head_commit = Commit::parse(head.filepath().string());
      auto source_revisions = source_commit.revision_history();
      const auto head_revisions = head_commit.revision_history();
      fs::create_directory(fs::absolute(lit::MERGE_SOURCE));
      fs::create_directory(fs::absolute(lit::MERGE_TARGET));

      for (auto& r : source_revisions) {
        Patch::apply(fs::absolute(r.patchpath()), fs::absolute(lit::MERGE_SOURCE));
      }
      for (auto& r : head_revisions) {
        Patch::apply(fs::absolute(r.patchpath()), fs::absolute(lit::MERGE_TARGET));
      }

      // TODO: refactor
      Revision base_rev = head_revisions.at(0);
      for (auto i = 0; i < head_revisions.size(); i++) {
        try {
          if (source_revisions.at(i) == head_revisions.at(i)) {
            base_rev = head_revisions.at(i);
          }
        } catch (std::out_of_range const& exc) {}
      }
      lit::Repository::checkout(base_rev);
      cout << "Base for both branches is: " << base_rev.to_string() << endl;

      const Diff source_diff(fs::absolute(lit::MERGE_SOURCE), true);
      const Diff head_diff(fs::absolute(lit::MERGE_TARGET), true);

      if (false) {
        cerr << "Merge conflict." << endl;
        return 1;
      }


      cout << "Merging " << source.to_string() << " into " << head.to_string() << '.' << endl;

      lit::Repository::checkout(head);

      const auto base_revisions = Commit::parse(base_rev.filepath().string()).revision_history();
      for (auto& r : base_revisions) {
        source_revisions.pop_front();
      }
      for (auto& r : source_revisions) {
        Patch::apply(r.patchpath());
      }

      cout << "Succesfully merged changes from " << source.to_string() << '.' << endl;
      cout << "Please commit changes now." << endl;

      return 0;
    }
};