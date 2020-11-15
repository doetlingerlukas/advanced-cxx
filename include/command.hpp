#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class Command {
  public:
  virtual string name() const {
    return "lit";
  }

  virtual string description() const {
    return "A basic version control system";
  }

  virtual int execute(vector<string> arguments) const {
    return 0;
  };

  static map<string, unique_ptr<Command>> create_map();
};