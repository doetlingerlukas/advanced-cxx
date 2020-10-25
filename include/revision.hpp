#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include <regex>

#include <constants.hpp>

namespace fs = std::filesystem;

using namespace std;

class Revision {
  unsigned long long id_;

  public:
    Revision(string r) {
      const regex revision_regex("r[0-9]+");

      if (!regex_match(r, revision_regex)) {
        cerr << r << " is not a correct revision!" << endl;
        abort();
      }

      r.erase(0, 1);
      id_ = stoull(r, nullptr, 10);
    }

    Revision(const unsigned long long& id) {
      id_ = id;
    }

    unsigned long long id() const {
      return id_;
    }

    string to_string() const {
      return "r" + id_;
    }

    fs::path directory() const {
      return fs::path(lit::REVISION_DIR) / this->to_string();
    }

    fs::path filepath() const {
      return this->directory() / ".lit";
    }

    fs::path patchpath() const {
      return this->directory() / ".patch";
    }

    static Revision from_id(const string id_string) {
      return Revision(stoull(id_string, nullptr, 10));
    }
};