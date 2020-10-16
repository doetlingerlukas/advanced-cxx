#include <string>
#include <map>
#include <memory>

#include <command.hpp>
#include <help.hpp>
#include <init.hpp>

map<string, unique_ptr<Command>> Command::create_map() {
  map<string, unique_ptr<Command>> commands;

  commands.insert(make_pair("help", unique_ptr<Command>(new Help())));
  commands.insert(make_pair("init", unique_ptr<Command>(new Init())));

  return commands;
}