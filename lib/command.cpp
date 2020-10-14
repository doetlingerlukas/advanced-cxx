#include <string>
#include <map>
#include <memory>

#include <command.hpp>
#include <help.hpp>

map<string, unique_ptr<Command>> Command::create_map() {
  map<string, unique_ptr<Command>> commands;

  commands.insert(make_pair("help", unique_ptr<Command>(new Help())));

  return commands;
}