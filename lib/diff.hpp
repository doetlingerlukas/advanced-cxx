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

using namespace std;

class Diff {
  public:
    string content;

    Diff() {
      array<char, 128> buffer;
      string result;
      unique_ptr<FILE, decltype(&pclose)> pipe(popen("diff --exclude=.lit -ruN lit .", "r"), pclose);
      if (!pipe) {
          throw runtime_error("popen() failed!");
      }
      while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
          result += buffer.data();
      }

      content = result;
    }

    void save(string filepath) const {
      ofstream file;
      file.open(filepath);
      file << this->content;
      file.close();
    }
};