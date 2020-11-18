#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include <constants.hpp>
#include <subprocess.hpp>

namespace fs = std::filesystem;

namespace lit {

using namespace std;

class Diff {
  string content_;
  map<string, char> status_;

  public:
  Diff(fs::path directory, bool merge_dir) {
    for (auto& p : fs::recursive_directory_iterator(directory)) {
      if (!(p.is_directory() || contains_lit_path(p)) || (!p.is_directory() && merge_dir)) {
        const auto pair = system_popen(build_command(directory, p));
        content_ += pair.second;
        if (pair.first != 0) status_.insert(status_pair(p, directory));
      }
    }

    for (auto& p : fs::recursive_directory_iterator(fs::absolute(PREVIOUS_DIR))) {
      const auto relative = fs::relative(p, fs::absolute(PREVIOUS_DIR));
      if (!(p.is_directory() || fs::exists(directory / relative))) {
        content_ += system_popen(build_command(directory, directory / relative)).second;
        status_.insert(make_pair(relative.string(), 'D'));
      }
    }
  }

  Diff() : Diff(fs::current_path(), false) {}

  map<string, char> status() const {
    return this->status_;
  }

  void save(const string& filepath) const {
    ofstream file;
    file.open(filepath);
    file << this->content_;
    file.close();
  }

  void print_status() const {
    for (auto& p : status_) {
      cout << p.second << " " << p.first << endl;
    }
  }

  bool status_contains(const string& key) const {
    auto search = this->status_.find(key);
    return (search == this->status_.end()) ? false : true;
  }

  map<string, char> conflicts(const Diff& diff_) const {
    map<string, char> conflicts_;
    for (auto& s : this->status_) {
      if (diff_.status_contains(s.first)) {
        conflicts_.insert(s);
      }
    }
    return conflicts_;
  }

  bool conflicts_with(const Diff& diff_) const {
    return !conflicts(diff_).empty();
  }

  private:
  static string build_command(const fs::path& wd, const fs::path& file) {
    return "diff -uN " + string(PREVIOUS_DIR) + "/" + fs::relative(file, wd).string() + " " +
           fs::relative(file, fs::current_path()).string();
  }

  static pair<string, char> status_pair(const fs::directory_entry& p, const fs::path& wd) {
    auto relative = fs::relative(p, wd);
    if (fs::exists(PREVIOUS_DIR / relative)) {
      return make_pair(relative.string(), 'M');
    }
    return make_pair(relative.string(), 'A');
  }

  static bool contains_lit_path(const fs::path& base) {
    auto sub = fs::absolute(DIR);
    return search(base.begin(), base.end(), sub.begin(), sub.end()) != base.end();
  }
};

} // namespace lit