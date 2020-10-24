#pragma once

#include <string>
#include <filesystem>
#include <memory>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>

#include <constants.hpp>
#include <sub-process.hpp>

namespace fs = std::filesystem;

namespace std {
  bool contains_lit_path(const fs::path& base) {
    auto sub = fs::absolute(lit::DIR);
    return search(base.begin(), base.end(), sub.begin(), sub.end()) != base.end();
  }

  class Diff {
    public:
      string content;
      map<string, char> status;

      Diff() {
        const auto current_path = fs::current_path();

        for (auto& p: fs::recursive_directory_iterator(current_path)) {
          if (!(p.is_directory() || contains_lit_path(p))) {
            auto pair = system_popen(build_command(fs::relative(p, current_path).string()));
            content += pair.second;
            if (pair.first == 1) status.insert(get_status(p));
          }
        }

        auto previous = fs::absolute(lit::PREVIOUS_DIR);
        for (auto& p : fs::recursive_directory_iterator(previous)) {
          auto relative = fs::relative(p, previous);
          if (!(p.is_directory() || fs::exists(current_path / relative))) {
            content += system_popen(build_command(relative.string())).second;
            status.insert(make_pair(relative.string(), 'D'));
          }
        }
      }

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

    private:
      static string build_command(const string& file) {
        return "diff -uN " + string(lit::PREVIOUS_DIR) + "/" + file + " " + file;
      }

      static pair<string, char> get_status(const fs::directory_entry& p) {
        auto relative = fs::relative(p, fs::current_path());
        auto previous = lit::PREVIOUS_DIR / relative;
        if (fs::exists(previous)) {
          return make_pair(relative.string(), 'M');
        }
        return make_pair(relative.string(), 'A');
      }
  };
}