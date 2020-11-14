#pragma once

#include <fstream>
#include <string>
#include <filesystem>
#include <optional>

#include <constants.hpp>
#include <revision.hpp>
#include <commit.hpp>
#include <patch.hpp>

namespace fs = std::filesystem;

namespace lit {

using namespace std;

class Repository {
  public:
    static vector<Revision> revisions() {
      vector<Revision> revisions_;
      for (auto r = 0; r < Repository::current_index().value_or(0); r++) {
        revisions_.push_back(Revision(r));
      }
      return revisions_;
    }

    static optional<Revision> get_head() {
      ifstream head_file;
      head_file.open(string(HEAD));

      string head_revision;
      getline(head_file, head_revision);
      head_file.close();

      if (head_revision.empty()) {
        return nullopt;
      }
      return optional(Revision::from_id(head_revision));
    }

    static void set_head(const Revision& revision) {
      ofstream head_file;
      head_file.open(string(HEAD), ofstream::out | ofstream::trunc);
      head_file << revision.id();
      head_file.close();
    }

    static optional<unsigned long long> current_index() {
      ifstream index_file;
      index_file.open(INDEX.data());

      string index;
      getline(index_file, index);
      index_file.close();

      if (index.empty()) {
        return nullopt;
      }
      return optional(stoull(index));
    }

    static unsigned long long update_index() {
      auto current = Repository::current_index();
      auto updated = current.has_value() ? current.value() + 1ULL : 0ULL;

      ofstream index_file;
      index_file.open(INDEX.data(), ofstream::out | ofstream::trunc);
      index_file << updated;
      index_file.close();

      return updated;
    }

    static void clear_previous_dir() {
      for (auto& p : fs::directory_iterator(PREVIOUS_DIR)) {
        fs::remove_all(p);
      }
    }

    static void set_previous_dir() {
      Repository::clear_previous_dir();
      
      for (auto& p : fs::directory_iterator(fs::current_path())) {
        if (p.path().filename().string() != DIR) {
          const auto copy_options = fs::copy_options::overwrite_existing
            | fs::copy_options::recursive;
          fs::copy(p, fs::absolute(PREVIOUS_DIR) / fs::relative(p.path(), fs::current_path()), copy_options);
        }
      }
    }

    static void clear() {
      Repository::clear_previous_dir();

      for (auto& p : fs::directory_iterator(fs::current_path())) {
        if (p.path().filename().string() != DIR) {
          fs::remove_all(p);
        }
      }
    }

    static void checkout(const Revision& revision) {
      Commit commit = Commit::parse(revision.filepath().string());
      auto revisions = commit.revision_history();

      Repository::clear();
      for (auto& r : revisions) {
        Patch::apply(r.patchpath());
      }
      Repository::set_previous_dir();
      Repository::set_head(revision);
    }
};

} // namespace lit
