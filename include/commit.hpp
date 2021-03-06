#pragma once

#include <chrono>
#include <ctime>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

#include <revision.hpp>

namespace lit {

using namespace std;

class Commit {
  Revision revision_;
  optional<Revision> parent_;
  optional<Revision> merge_parent_;
  string message_;
  chrono::system_clock::time_point timestamp_;

  public:
  Commit(Revision revision, optional<Revision> parent, optional<Revision> merge_parent, const string& message,
         const chrono::system_clock::time_point& timestamp)
      : revision_(revision), parent_(parent), merge_parent_(merge_parent), message_(message), timestamp_(timestamp) {}

  Commit(Revision revision, optional<Revision> parent, optional<Revision> merge_parent, const string& message)
      : Commit(revision, parent, merge_parent, message, chrono::system_clock::now()) {}

  Revision revision() const {
    return this->revision_;
  }

  optional<Revision> parent() const {
    return this->parent_;
  }

  optional<Revision> merge_parent() const {
    return this->merge_parent_;
  }

  string message() const {
    return this->message_;
  }

  chrono::system_clock::time_point timestamp() const {
    return this->timestamp_;
  }

  deque<Revision> revision_history() const {
    deque<Revision> parents_;
    auto p = this->parent_;

    while (p.has_value()) {
      parents_.push_front(p.value());
      p = Commit::parse(p.value().filepath().string()).parent();
    }

    parents_.push_back(this->revision_);
    return parents_;
  }

  void print() const {
    cout << "Commit: " << this->revision_.to_string() << endl;

    cout << "Parents: ";
    if (this->parent_.has_value()) {
      cout << this->parent_.value().to_string();
    }
    if (this->merge_parent_.has_value()) {
      cout << " " << this->merge_parent_.value().to_string();
    }
    cout << endl;

    auto time = chrono::system_clock::to_time_t(this->timestamp_);
    cout << "Date: " << ctime(&time) << endl;
    cout << endl << this->message_ << endl;
  }

  void save() const {
    ofstream file;
    file.open(this->revision_.filepath().string());

    file << this->revision_.id() << endl;

    if (this->parent_.has_value()) {
      file << this->parent_.value().id();
    }
    file << endl;

    if (this->merge_parent_.has_value()) {
      file << this->merge_parent_.value().id();
    }
    file << endl;

    file << this->message_ << endl;
    file << chrono::system_clock::to_time_t(this->timestamp_) << endl;

    file.close();
  }

  static Commit parse(const string& filepath) {
    ifstream file;
    file.open(filepath);

    string str;

    getline(file, str);
    const auto revision = Revision::from_id(str);

    getline(file, str);
    const auto parent = str.empty() ? nullopt : optional(Revision::from_id(str));

    getline(file, str);
    const auto merge_parent = str.empty() ? nullopt : optional(Revision::from_id(str));

    getline(file, str);
    const auto message = str;

    getline(file, str);
    const auto timestamp = chrono::system_clock::from_time_t(stoul(str));

    file.close();
    return Commit(revision, parent, merge_parent, message, timestamp);
  }
};

} // namespace lit