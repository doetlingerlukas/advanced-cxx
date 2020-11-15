#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <command.hpp>

using namespace std;

int main(int argc, const char* argv[]) {
  vector<string> args(argv + 1, argv + argc);

  if (args.empty()) {
    cout << "Please supply a command!" << endl;
    return 0;
  }

  auto commands = Command::create_map();

  auto search = commands.find(args.front());
  if (search == commands.end()) {
    cerr << "Unknown command: " << args.front() << endl;
    return 1;
  } else {
    search->second->execute(vector<string>(args.begin() + 1, args.end()));
  }

  return 0;
}