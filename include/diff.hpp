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

bool contains_lit_path(const fs::path& base) {
  auto sub = fs::absolute(DIR);
  return search(base.begin(), base.end(), sub.begin(), sub.end()) != base.end();
}

class Diff {
  public:
  string content;
  map<string, char> status;

  Diff(fs::path directory, bool merge_dir) {
    for (auto& p : fs::recursive_directory_iterator(directory)) {
      if (!(p.is_directory() || contains_lit_path(p)) || (!p.is_directory() && merge_dir)) {
        auto pair = system_popen(build_command(directory, p));
        content += pair.second;
        if (pair.first != 0) status.insert(get_status(p, directory));
      }
    }

    auto previous = fs::absolute(PREVIOUS_DIR);
    for (auto& p : fs::recursive_directory_iterator(previous)) {
      auto relative = fs::relative(p, previous);
      if (!(p.is_directory() || fs::exists(directory / relative))) {
        content += system_popen(build_command(directory, directory / relative)).second;
        status.insert(make_pair(relative.string(), 'D'));
      }
    }
  }

  Diff() : Diff(fs::current_path(), false) {}

  void save(const string& filepath) const {
    ofstream file;
    file.open(filepath);
    file << this->content;
    file.close();
  }

  void print_status() const {
    for (auto& p : status) {
      cout << p.second << " " << p.first << endl;
    }
  }

  bool status_contains(const string& key) const {
    auto search = this->status.find(key);
    return (search == this->status.end()) ? false : true;
  }

  map<string, char> conflicts(const Diff& diff_) const {
    map<string, char> conflicts_;
    for (auto& s : this->status) {
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

  static pair<string, char> get_status(const fs::directory_entry& p, const fs::path& wd) {
    auto relative = fs::relative(p, wd);
    auto previous = PREVIOUS_DIR / relative;
    if (fs::exists(previous)) {
      return make_pair(relative.string(), 'M');
    }
    return make_pair(relative.string(), 'A');
  }
};

} // namespace lit