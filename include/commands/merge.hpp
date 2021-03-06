#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <command.hpp>
#include <commit.hpp>
#include <constants.hpp>
#include <diff.hpp>
#include <patch.hpp>
#include <revision.hpp>

namespace fs = std::filesystem;

namespace lit {

using namespace std;

class MergeCommand : public Command {
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

    cout << "Starting merge." << endl;

    const auto source = Revision(arguments.front());
    const auto head = Repository::get_head().value_or(Revision(0));
    if (!(source.exists() && head.exists()) || source == head) {
      cerr << "Incompatiple commit supplied." << endl;
      cerr << source.to_string() << " cannot be merged into HEAD " << head.to_string() << endl;
      return 1;
    }

    const auto source_commit = Commit::parse(source.filepath().string());
    const auto head_commit = Commit::parse(head.filepath().string());
    const auto source_revisions = source_commit.revision_history();
    const auto head_revisions = head_commit.revision_history();
    fs::create_directory(fs::absolute(MERGE_SOURCE));
    fs::create_directory(fs::absolute(MERGE_TARGET));

    for (auto& r : source_revisions) {
      Patch::apply(fs::absolute(r.patchpath()), fs::absolute(MERGE_SOURCE));
    }
    for (auto& r : head_revisions) {
      Patch::apply(fs::absolute(r.patchpath()), fs::absolute(MERGE_TARGET));
    }

    Revision base_rev = head_revisions.at(0);
    for (auto i = 0; i < head_revisions.size(); i++) {
      auto commit = Commit::parse(head_revisions.at(i).filepath().string());
      try {
        if (commit.merge_parent().has_value()) {
          if (std::find(source_revisions.begin(), source_revisions.end(), commit.merge_parent().value()) !=
              source_revisions.end()) {
            base_rev = commit.merge_parent().value();
          }
        } else if (source_revisions.at(i) == head_revisions.at(i)) {
          base_rev = head_revisions.at(i);
        }
      } catch (std::out_of_range const& exc) {
      }
    }
    Repository::checkout(base_rev);
    cout << "Base for both branches is: " << base_rev.to_string() << endl;

    const Diff source_diff(fs::absolute(MERGE_SOURCE), true);
    const Diff head_diff(fs::absolute(MERGE_TARGET), true);

    if (head_diff.conflicts_with(source_diff)) {
      const auto conflicts = head_diff.conflicts(source_diff);

      cout << "Merge conflict(s) detected." << endl;

      // Copy conflicts files
      const auto copy_option = fs::copy_options::overwrite_existing;
      for (auto& c : conflicts) {
        cout << "\t - " << c.first << endl;

        if (fs::exists(fs::absolute(MERGE_SOURCE) / fs::path(c.first))) {
          const auto target_file = fs::current_path() / fs::path(c.first + "." + source.to_string());
          fs::create_directory(target_file.parent_path());
          fs::copy(fs::absolute(MERGE_SOURCE) / fs::path(c.first), target_file, copy_option);
        }

        fs::rename(fs::current_path() / fs::path(c.first),
                   fs::current_path() / fs::path(c.first + "." + base_rev.to_string()));
      }

      for (auto& f : head_diff.status()) {
        copy_file(f, fs::absolute(MERGE_TARGET));
      }

      for (auto& f : source_diff.status()) {
        if (!head_diff.status_contains(f.first)) {
          copy_file(f, fs::absolute(MERGE_SOURCE));
        }
      }

      Repository::set_head(head);
      Repository::set_parent_for_merge_conflict(source);
    } else {
      cout << "Merging " << source.to_string() << " into " << head.to_string() << '.' << endl;

      Repository::checkout(head);

      for (auto& f : source_diff.status()) {
        copy_file(f, fs::absolute(MERGE_SOURCE));
      }

      cout << "Succesfully merged changes from " << source.to_string() << '.' << endl;

      Revision revision(Repository::update_index());
      fs::create_directories(revision.directory());

      Diff diff;
      diff.save(revision.patchpath().string());

      Commit commit(revision, optional<Revision>(head), optional<Revision>(source),
                    ("Merge " + source.to_string() + " into " + head.to_string() + '.'));
      commit.save();

      Repository::set_head(revision);
      Repository::set_previous_dir();
    }

    fs::remove_all(fs::absolute(MERGE_SOURCE));
    fs::remove_all(fs::absolute(MERGE_TARGET));

    return 0;
  }

  private:
  // Copy file to current branch
  void copy_file(const pair<string, char>& file, const fs::path& location) const {
    const auto copy_option = fs::copy_options::overwrite_existing;
    const auto target_file = fs::current_path() / fs::path(file.first);
    if (file.second == 'D') {
      fs::remove(target_file);
    } else {
      fs::create_directory(target_file.parent_path());
      fs::copy(location / fs::path(file.first), target_file, copy_option);
    }
  }
};

} // namespace lit