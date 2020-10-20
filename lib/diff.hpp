#pragma once

#ifdef __WIN32__
#define popen  _popen
#define pclose _pclose
#endif

#include <string>
#include <filesystem>
#include <cstdio>
#include <array>
#include <memory>
#include <iostream>
#include <fstream>
#include <map>

namespace fs = std::filesystem;

using namespace std;

pair<int, string> execute_diff(string file) {
  array<char, 128> buffer;
  string result;

  auto pipe = popen(string("diff -uN .lit/previous/" + file + " " + file).c_str(), "r");

  if (!pipe) throw runtime_error("popen() failed!");

  while (!feof(pipe)) {
    if (fgets(buffer.data(), 128, pipe) != nullptr) result += buffer.data();
  }

  auto status = pclose(pipe);

  return make_pair(status, result);
}

class Diff {
  public:
    string content;
    map<string, char> status_map;

    Diff() {
      auto current_path = fs::current_path();

      for (auto& p : fs::directory_iterator(current_path)) {
        if (p.is_directory() && p.path().filename().string() != ".lit") {
          for (auto& sp : fs::recursive_directory_iterator(p)) {
            if (!sp.is_directory()) {
              auto pair = execute_diff(fs::relative(sp.path(), current_path).string());
              content += pair.second;
              if (pair.first == 1) status_map.insert(this->add_to_status(sp));
            }
          }
        } else if (!p.is_directory()) {
          auto pair = execute_diff(fs::relative(p.path(), current_path).string());
          content += pair.second;
          if (pair.first == 1) status_map.insert(this->add_to_status(p));
        }
      }

      auto previous_dir = fs::absolute(fs::path(".lit/previous"));
      for (auto& p : fs::recursive_directory_iterator(previous_dir)) {
        if (!p.is_directory()) {
          auto relative = fs::relative(p.path(), previous_dir);
          if (!fs::exists(current_path / relative)) {
            content += execute_diff(relative.string()).second;
            status_map.insert(make_pair(relative.string(), 'D'));
          }
        }
      }
    }

    void save(string filepath) const {
      ofstream file;
      file.open(filepath);
      file << this->content;
      file.close();
    }

    void print_status() const {
      for (auto& p : status_map) {
        cout << p.second << " " << p.first << endl;
      }
    }

  private:
    static pair<string, char> add_to_status(const fs::directory_entry& p) {
      auto relative = fs::relative(p.path(), fs::current_path());
      auto previous = ".lit/previous/" / relative;
      if (fs::exists(previous)) {
        return make_pair(relative.string(), 'M');
      }
      return make_pair(relative.string(), 'A');
    }
};