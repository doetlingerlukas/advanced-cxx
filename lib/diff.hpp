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

string execute_diff(string file) {
  array<char, 128> buffer;
  string result;
  unique_ptr<FILE, decltype(&pclose)> pipe(popen(string("diff -uN .lit/previous/" + file + " " + file).c_str(), "r"), pclose);
  if (!pipe) {
    throw runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

class Diff {
  public:
    string content;

    Diff() {
      auto current_path = fs::current_path();

      for (auto& p : fs::directory_iterator(current_path)) {
        if (p.is_directory() && p.path().filename().string() != ".lit") {
          for (auto& sp : fs::recursive_directory_iterator(p)) {
            if (!sp.is_directory()) {
              content += execute_diff(fs::relative(sp.path(), current_path).string());
            }
          }
        } else if (!p.is_directory()) {
          content += execute_diff(fs::relative(p.path(), current_path).string());
        }
      }
    }

    void save(string filepath) const {
      ofstream file;
      file.open(filepath);
      file << this->content;
      file.close();
    }
};